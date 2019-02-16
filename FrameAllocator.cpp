/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FrameAllocator.cpp
 * Author: Evan Derby & Jason Ghiglieri
 * 
 * Created on January 28, 2019, 5:22 PM
 */

#include "FrameAllocator.h"

#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>


FrameAllocator::FrameAllocator(unsigned long page_count) {
    this->memory = std::vector<uint8_t>(page_count * PAGE_FRAME_SIZE);
    
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
           store_in_memory(0xFFFFFFFF, current_address);
           break;
       }
       store_in_memory(next_address, current_address);
       current_address = next_address;
    }
}

FrameAllocator::~FrameAllocator() {
}

bool FrameAllocator::allocate(uint32_t count, std::vector<uint32_t> &page_frames) {
    if (count > get_avaliable()) {
        return false;
    }
    uint32_t address = 0;
    uint32_t old_address = 0;
    int address_count = count;
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
    return true;
}

bool FrameAllocator::release(uint32_t count, std::vector<uint32_t> &page_frames) {
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
    while (read_address != 0xFFFFFFFF) {
        list_string << std::hex << read_address << " ";
        load_from_memory(read_address, read_address);
    }
    return list_string.str();
}

void FrameAllocator::store_in_memory(uint32_t value, uint32_t address) {
    memcpy(&(this->memory[address]), &value, sizeof(uint32_t));
}

void FrameAllocator::load_from_memory(uint32_t &value, uint32_t address) const {
    memcpy(&value, &(this->memory[address]), sizeof(uint32_t));
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

void FrameAllocator::print_memory(uint32_t start_address, uint32_t end_address) const {
    int byte_counter = 0;
    for (uint32_t i = start_address; i <= end_address; i++) {
        if (byte_counter % 8 == 0) {
            std::cout << std::setfill('0') << std::setw(7) << std::hex << start_address+i << ": "; 
        }
        std::cout << std::setfill('0') << std::setw(2) << std::hex 
                << static_cast<int>(this->memory[start_address+i]) << " ";
        byte_counter++;
        if (byte_counter == 8) {
            std::cout << std::endl;
            byte_counter = 0;
        }
    }
    std::cout << std::endl;
}