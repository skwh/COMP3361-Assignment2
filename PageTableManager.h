/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PageTableManager.h
 * Author: jason & evan
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
    /**
     * Creates the page table manager with references to an MMU and a FrameAllocator.
     * Also initializes the kernel page table and sets the MMU to virtual mode.
     * @param mem the MMU to use
     * @param alloc the FrameAllocator to use
     */
    PageTableManager(mem::MMU& mem, FrameAllocator& alloc);
    /*
     * Delete copy and assignment operators.
     */
    PageTableManager(const PageTableManager& orig) = delete;
    PageTableManager(PageTableManager && orig) = delete;
    PageTableManager operator=(const PageTableManager & orig) = delete;
    PageTableManager operator=(PageTableManager && orig) = delete;
    
    virtual ~PageTableManager();
    
    /**
     * Set the MMU to kernel mode and return the PMCB that was in use.
     * @return the PMCB in use in the MMU
     */
    mem::PMCB set_kernel_mode();
    
    /**
     * Allocate a page table for a process.
     * @return the process_id associated with the table
     */
    int allocate_process_page_table();
    /**
     * Create page table entries for count frames being allocated
     * @param vaddr address to begin allocating at
     * @param count number of entries to allocate
     */
    void map_process_table_entries(uint32_t vaddr, int count);
    /**
     * Set the entry permissions for count frames
     * @param vaddr address to begin modifying at
     * @param count number of entries to modify
     * @param setting true to set the writable bit, false to clear it
     */
    void set_page_permissions(uint32_t vaddr, int count, bool setting);
    /**
     * Set the MMU to use the page_table (PMCB) associated with the given process_id
     * @param process_id the process_id to search for
     * @return if the operation succeeded or not- will fail if process_id is not found
     */
    bool set_process_page_table(int process_id);
    /**
     * Remove the page table associated with process_id from the list
     * @param process_id the process_id to search for
     * @return if the operation succeeded or not- will fail if process_id is not found
     */
    bool remove_process_page_table(int process_id);
    
private:
    // Current count for process IDs, used in assigning
    int current_process_id_count;
    
    // MMU reference
    mem::MMU& memory;
    // FrameAllocator reference
    FrameAllocator & frame_allocator;
    // List of page tables and associated process_ids
    std::map<int, mem::PMCB> process_page_tables;
};

#endif /* PAGETABLEMANAGER_H */

