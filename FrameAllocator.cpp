/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FrameAllocator.cpp
 * Author: evan
 * 
 * Created on January 28, 2019, 5:22 PM
 */

#include "FrameAllocator.h"

#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>


FrameAllocator::FrameAllocator(unsigned long page_count, mem::MMU* mem) {
    this->memory = mem;
    
    this->memory->set_kernel_PMCB();
    //store the page count in the memory
    store_in_memory(page_count, PAGE_FRAMES_TOTAL_OFFSET);
    //store the avaliable page frames in memory: the 0th frame is reserved
    store_in_memory(page_count - 1, PAGE_FRAMES_AVALIABLE_OFFSET);
    //store the address of the next page frame in memory, always the page frame size
    store_in_memory(PAGE_FRAME_SIZE, AVALIABLE_LIST_HEAD_OFFSET);
    
    //create the linked list structure
    uint32_t current_address = PAGE_FRAME_SIZE;
    while (current_address < page_count * PAGE_FRAME_SIZE) {
       uint32_t next_address = current_address + PAGE_FRAME_SIZE;
       if (next_address >= page_count * PAGE_FRAME_SIZE) {
           store_in_memory(LIST_END_POINT, current_address);
           break;
       }
       store_in_memory(next_address, current_address);
       current_address = next_address;
    }
}

FrameAllocator::~FrameAllocator() {
}

bool FrameAllocator::allocate(uint32_t count, std::vector<uint32_t> &page_frames, uint32_t vaddr) {
    this->memory->set_kernel_PMCB();
    if (count > get_avaliable()) {
        return false;
    }
    uint32_t address = 0;
    uint32_t old_address = 0;
    int address_count = count;
    if (vaddr == LIST_END_POINT) {
        get_head(address);
        while (address_count > 0) {
            page_frames.push_back(address);
            old_address = address;
            load_from_memory(address, address);
            set_frame_pointer(0, old_address);
            address_count--;
        }
        set_frames_avaliable(get_avaliable() - count);
        set_head(address);
    } else {
        address = vaddr >> 14;
        int prestart_frame_address = address - PAGE_FRAME_SIZE;
        // here we decouple frames from inside of the list instead of from its head
        while (address_count > 0) {
            page_frames.push_back(address);
            old_address = address;
            load_from_memory(address, address);
            set_frame_pointer(0, old_address);
            address_count--;
        }
        set_frame_pointer(address, prestart_frame_address);
        set_frames_avaliable(get_avaliable() - count);
    }
    return true;
}

bool FrameAllocator::release(uint32_t count, std::vector<uint32_t> &page_frames) {
    this->memory->set_kernel_PMCB();
    if (count > page_frames.size()) {
        return false;
    }
    uint32_t end_address;
    get_head(end_address);
    int address_count = count;
    while (address_count > 0) {
        uint32_t new_frame = page_frames.back();
        set_frame_pointer(end_address, new_frame);
        end_address = new_frame;
        set_head(end_address);
        page_frames.pop_back();
        address_count--;
    }
    set_frames_avaliable(get_avaliable() + count);
    return true;
}

uint32_t FrameAllocator::get_avaliable() const {
    uint32_t count = 0;
    load_from_memory(count, PAGE_FRAMES_AVALIABLE_OFFSET);
    return count;
}

std::string FrameAllocator::get_avaliable_list_string() const {
    std::stringstream list_string;
    uint32_t read_address;
    load_from_memory(read_address, AVALIABLE_LIST_HEAD_OFFSET);
    while (read_address != LIST_END_POINT) {
        list_string << std::hex << read_address << " ";
        load_from_memory(read_address, read_address);
    }
    return list_string.str();
}

void FrameAllocator::store_in_memory(uint32_t value, uint32_t address) {
    this->memory->movb(address, &value, sizeof(uint32_t));
}

void FrameAllocator::load_from_memory(uint32_t &value, uint32_t address) const {
    this->memory->movb(&value, address, sizeof(uint32_t));
}

void FrameAllocator::set_frames_avaliable(uint32_t value) {
    store_in_memory(value, PAGE_FRAMES_AVALIABLE_OFFSET);
}

void FrameAllocator::set_frame_pointer(uint32_t value, uint32_t address) {
    store_in_memory(value, address);
}

void FrameAllocator::get_head(uint32_t &value) const {
    load_from_memory(value, AVALIABLE_LIST_HEAD_OFFSET);
}

void FrameAllocator::set_head(uint32_t &value) {
    store_in_memory(value, AVALIABLE_LIST_HEAD_OFFSET);
}