/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PageTableManager.h
 * Author: jason
 *
 * Created on February 25, 2019, 2:19 PM
 */

#ifndef PAGETABLEMANAGER_H
#define PAGETABLEMANAGER_H

#include <MMU.h>
#include "FrameAllocator.h"
#include <map>

class PageTableManager {
    
public:
    PageTableManager(mem::MMU& mem, FrameAllocator& alloc);
    
    PageTableManager(const PageTableManager& orig) = delete;
    PageTableManager(PageTableManager && orig) = delete;
    PageTableManager operator=(const PageTableManager & orig) = delete;
    PageTableManager operator=(PageTableManager && orig) = delete;
    
    virtual ~PageTableManager();
    
    mem::PMCB set_kernel_mode();
    
    int allocate_process_page_table();
    void map_process_table_entries(uint32_t vaddr, int count);
    void set_page_permissions(uint32_t vaddr, int count, bool setting);
    bool set_process_page_table(int process_id);
    bool remove_process_page_table(int process_id);
    
private:
    int current_process_id_count;
    
    mem::MMU& memory;
    FrameAllocator & frame_allocator;
    std::map<int, mem::PMCB> process_page_tables;
};

#endif /* PAGETABLEMANAGER_H */

