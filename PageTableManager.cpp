/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PageTableManager.cpp
 * Author: jason & evan
 * 
 * Created on February 25, 2019, 2:20 PM
 */

#include "PageTableManager.h"

PageTableManager::PageTableManager(mem::MMU& mem, FrameAllocator& alloc):memory{mem},frame_allocator{alloc} {
    current_process_id_count = 1;
    
    std::vector<uint32_t> addresses;
    frame_allocator.allocate(1, addresses);
    
    mem::PageTable kernel_page_table;
    uint32_t num_pages = memory.get_frame_count();
    
    for (mem::Addr i = 0; i < num_pages; i++) {
        kernel_page_table.at(i) = 
                (i << mem::kPageSizeBits) | mem::kPTE_PresentMask | mem::kPTE_WritableMask;
    }
    
    memory.movb(addresses[0], &kernel_page_table, mem::kPageTableSizeBytes);
    
    uint32_t kernel_page_table_address = addresses[0];
    
    mem::PMCB kernel_pmcb(kernel_page_table_address);
    memory.enter_virtual_mode(kernel_pmcb);
}

PageTableManager::~PageTableManager() {
}

mem::PMCB PageTableManager::set_kernel_mode() {
    return memory.set_kernel_PMCB();
}

int PageTableManager::allocate_process_page_table() {
    mem::PageTable new_page_table;
    // Allocate a frame for the new user page table
    std::vector<uint32_t> allocated_frames;
    frame_allocator.allocate(1, allocated_frames);
    // Get the address of the allocated frame
    uint32_t page_table_base_address = allocated_frames.at(0);
    // Calculate the page table offset and write
    uint32_t pt_offset = (page_table_base_address >> mem::kPageSizeBits) & mem::kPageTableIndexMask;
    new_page_table.at(pt_offset) = page_table_base_address | mem::kPTE_PresentMask | mem::kPTE_WritableMask;
    // Ensure the MMU is in kernel mode
    memory.set_kernel_PMCB();
    // Write the process page table to memory
    memory.movb(page_table_base_address, &new_page_table, mem::kPageTableSizeBytes);
    // Create a new PMCB for this page table
    mem::PMCB user_pmcb(page_table_base_address);
    // Add the new PMCB to the list
    process_page_tables.insert(std::make_pair(current_process_id_count, user_pmcb));
    
    memory.set_user_PMCB(user_pmcb);
    
    int process_id = current_process_id_count;
    current_process_id_count++;
    return process_id;
}

void PageTableManager::map_process_table_entries(uint32_t vaddr, int count) {
    mem::PMCB pr_pmcb = set_kernel_mode();
    // Allocate count pages
    std::vector<uint32_t> allocated_frames;
    frame_allocator.allocate(count, allocated_frames);
    // Calculate the physical address from the virtual address
    uint32_t physical_address = ((vaddr >> mem::kPageSizeBits) * 4) + pr_pmcb.page_table_base;
    //Iterate over every page table entry and set the present and writable bits 
    for (int i = 0; i < count; i++) {
        allocated_frames.at(i) |= (mem::kPTE_PresentMask | mem::kPTE_WritableMask);
        //Write the page table entry to to memory
        memory.movb(physical_address, &allocated_frames.at(i), sizeof(mem::PageTableEntry));
        physical_address += 4;
    }
    
    memory.set_user_PMCB(pr_pmcb);
}

void PageTableManager::set_page_permissions(uint32_t vaddr, int count, bool setting) {
    mem::PMCB pr_pmcb = set_kernel_mode();
    uint32_t physical_address = ((vaddr >> mem::kPageSizeBits) * 4) + pr_pmcb.page_table_base;
    for (int i = 0; i < count; i++) {
        uint32_t page_table_entry;
        memory.movb(&page_table_entry, physical_address, sizeof(mem::PageTableEntry));
        if (setting) { //set the writable bit
            page_table_entry |= mem::kPTE_WritableMask;
        } else { // clear the writable bit
            page_table_entry &= mem::kPTE_WritableMask;
        }
        memory.movb(physical_address, &page_table_entry, sizeof(mem::PageTableEntry));
        physical_address += 4;
    }
    memory.set_user_PMCB(pr_pmcb);
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