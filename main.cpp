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
    
    PageTableManager table_manager(memory, allocator);
    
    Process trace(argv[1], &memory, &allocator, &table_manager);
    trace.Exec();
    
    return 0;
}

