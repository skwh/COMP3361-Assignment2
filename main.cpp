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

#include <cstdlib>
#include <iostream>

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
    FrameAllocator allocator(128, memory);
    
    std::vector<uint32_t> addresses;
    allocator.Allocate(1, addresses, memory);
    
    mem::PageTable kernel_page_table;
    mem::Addr num_pages = memory.get_frame_count();
    
    for (mem::Addr i = 0; i < num_pages; i++) {
        kernel_page_table.at(i) = 
                (i << mem::kPageSizeBits) | mem::kPTE_PresentMask | mem::kPTE_WritableMask;
    }
    
    memory.movb(addresses[0], &kernel_page_table, mem::kPageTableSizeBytes);
    
    mem::Addr kernelAddress = addresses[0];
    
    mem::PMCB kPMCB(kernelAddress);
    memory.enter_virtual_mode(kPMCB);
    
    Process trace(argv[1], &memory);
    trace.Exec();
    
    return 0;
}

