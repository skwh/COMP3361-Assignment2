/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FrameAllocator.h
 * Author: evan
 *
 * Created on January 28, 2019, 5:22 PM
 */

#ifndef FRAMEALLOCATOR_H
#define FRAMEALLOCATOR_H

#include <MMU.h>
#include <PMCB.h>

#include <vector>
#include <cstdint>
#include <string>

/*
 * Manages allocation and deallocation of page frames
 */
class FrameAllocator {
public:
    // Single Constructor requiring page_count parameter to initialize the memory vector
    FrameAllocator(unsigned long page_count, mem::MMU* mem);
    
    // Rule of 5 fulfilled by deleting the additional constructor types
    FrameAllocator(const FrameAllocator & orig) = delete;
    FrameAllocator(FrameAllocator && orig) = delete;
    FrameAllocator operator=(const FrameAllocator & orig) = delete;
    FrameAllocator operator=(FrameAllocator && orig) = delete;
    
    // empty destructor as specified
    virtual ~FrameAllocator();
    
    static const unsigned int LIST_END_POINT = 0xFFFFFFFF;
    
    /**
     * 
     * @param count: Number of page frames to allocate
     * @param page_frames: Vector on which the addresses allocated page frames 
     * should be pushed.
     * @return bool: returns true if the page frames were allocated successfully,
     *  returns false if there are not enough available page frames to allocate
     * all of the requested frames. If false, none of the page frames are allocated
     */
    bool allocate(uint32_t count, std::vector<uint32_t> &page_frames, uint32_t start_address = LIST_END_POINT);
    
    /**
     * 
     * @param count: The last count page frame addresses are to be popped from
     * the provided vector
     * @param page_frames: Vector from which page frames should be popped
     * @return bool: returns true and removes page frames as specified if count
     * is not greater than the total size of the provided vector. 
     * Returns false if more page frames are to be removed than exist in the 
     * vector and removes none of the page frames.
     */
    bool release(uint32_t count, std::vector<uint32_t> &page_frames);

    /**
     * @return uint32_t: Returns the available number of page frames from the 
     * private variables of the header file.
     */
    uint32_t get_avaliable() const;
    /**
     * 
     * @return std::string: Returns a string representation of the available
     * list of page frames
     */
    std::string get_avaliable_list_string() const;
    
    const unsigned long PAGE_FRAME_SIZE = 0x4000;
    
private:
    /*
     Inside of the memory array the following variables are stored:
     * PAGE_FRAMES_TOTAL (OFFSET 0, SIZE 32 BITS / 4 BYTES)
     * PAGE_FRAMES_AVALIABLE (OFFSET 4 BYTES, SIZE 4 BYTES)
     * AVALIABLE_LIST_HEAD (OFFSET 8 BYTES, SIZE 4 BYTES)
     */
    
    // Offset variables for repeated use
    const unsigned long PAGE_FRAMES_TOTAL_OFFSET = 0x0;
    const unsigned long PAGE_FRAMES_AVALIABLE_OFFSET = 0x4;
    const unsigned long AVALIABLE_LIST_HEAD_OFFSET = 0x8;
    
    // Memory
    mem::MMU* memory;
    
    /**
     * Helper method for storing a value in memory. Stores the given value at 
     * the given address
     * @param value: value to be stored in memory
     * @param address: address at which to store the above mentioned value
     */
    void store_in_memory(uint32_t value, uint32_t address);
    /**
     * Helper method for loading data from memory. Loads data from given address
     * and stores it in the given value variable
     * @param value: variable for the loaded data to be stored in
     * @param address: Address at which to load data from
     */
    void load_from_memory(uint32_t &value, uint32_t address) const;
    
    /**
     * Sets the value of the available page frames variable
     * @param value: value to set the available page frames variable to
     */
    void set_frames_avaliable(uint32_t value);
    /**
     * Sets pointer of the frame at given address to the given value
     * @param value: value to set frame pointer to
     * @param address: address of page frame  whose pointer is to be set
     */
    void set_frame_pointer(uint32_t value, uint32_t address);
    /**
     * Gets the head value and stores it in value
     * @param value: variable for the head value to be stored in
     */
    void get_head(uint32_t &value) const; 
    /**
     * Sets the head value to the given value
     * @param value: value the head shpuld be set to
     */
    void set_head(uint32_t &value);
};

#endif /* FRAMEALLOCATOR_H */

