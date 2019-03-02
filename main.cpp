/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: evan
 *
 * Created on February 16, 2019, 2:18 PM
 */

#include "Process.h"
#include "PageTableManager.h"
#include "PageFaultHandler.h"
#include "PermissionFaultHandler.h"

#include <cstdlib>
#include <iostream>
#include <memory>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " file_name " << std::endl;
        return 1;
    }
    
    mem::MMU memory(128);
    mem::Addr num_pages = memory.get_frame_count();
    FrameAllocator allocator(num_pages, &memory);
    
    PageFaultHandler page_fault_handler;
    PermissionFaultHandler permission_fault_handler;
    memory.SetPageFaultHandler(std::make_shared<PageFaultHandler>(page_fault_handler));
    memory.SetWritePermissionFaultHandler(std::make_shared<PermissionFaultHandler>(permission_fault_handler));
    
    std::vector<uint32_t> addresses;
    allocator.allocate(1, addresses);
    
    mem::PageTable kernel_page_table;
    
    for (mem::Addr i = 0; i < num_pages; i++) {
        kernel_page_table.at(i) = 
                (i << mem::kPageSizeBits) | mem::kPTE_PresentMask | mem::kPTE_WritableMask;
    }
    
    memory.movb(addresses[0], &kernel_page_table, mem::kPageTableSizeBytes);
    
    mem::Addr kernelAddress = addresses[0];
    
    mem::PMCB kPMCB(kernelAddress);
    memory.enter_virtual_mode(kPMCB);
    
    PageTableManager table_manager(memory, allocator);
    
    Process trace(argv[1], &memory, &allocator, &table_manager);
    trace.Exec();
    
    return 0;
}

