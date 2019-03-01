/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PageTableManager.cpp
 * Author: jason
 * 
 * Created on February 25, 2019, 2:20 PM
 */

#include "PageTableManager.h"

PageTableManager::PageTableManager(mem::MMU& mem, FrameAllocator& alloc):memory{mem},frame_allocator{alloc} {
    current_process_id_count = 1;
}

PageTableManager::~PageTableManager() {
}

int PageTableManager::allocate_process_page_table(bool set_active = true) {
    mem::PageTable new_page_table;
    std::vector<uint32_t> allocated_frames;
    frame_allocator.allocate(1, allocated_frames);
    uint32_t page_table_base_address = allocated_frames.at(0);
    uint32_t page_table_physical_address;
    memory.ToPhysical(page_table_base_address, page_table_physical_address, true);
    uint32_t pt_offset = (page_table_base_address >> mem::kPageSizeBits) & mem::kPageTableIndexMask;
    new_page_table.at(pt_offset) = page_table_physical_address | mem::kPTE_PresentMask | mem::kPTE_WritableMask;
    memory.set_kernel_PMCB();
    memory.movb(page_table_base_address, &new_page_table, mem::kPageTableSizeBytes);
    mem::PMCB user_pmcb(page_table_base_address);
    process_page_tables.insert(std::make_pair(current_process_id_count, user_pmcb));
    int process_id = current_process_id_count;
    if (set_active) {
        set_process_page_table(process_id);
    }
    current_process_id_count++;
    return process_id;
}

bool PageTableManager::set_process_page_table(int process_id) {
    std::map<int, mem::PMCB>::iterator found_table = process_page_tables.find(process_id);
    if (found_table == process_page_tables.end()) {
        return false; // The page table couldn't be found
    }
    mem::PMCB found_pmcb = (*found_table).second;
    memory.set_user_PMCB(found_pmcb);
    return true;
}

bool PageTableManager::remove_process_page_table(int process_id) {
    std::map<int, mem::PMCB>::iterator found_table = process_page_tables.find(process_id);
    if (found_table == process_page_tables.end()) {
        return false; // The page table couldn't be found
    }
    process_page_tables.erase(found_table);
    return true;
}