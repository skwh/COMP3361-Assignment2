/* FrameAllocator - allocate pages in memory
 * 
 * COMP3361 Winter 2019 - Lab 3 Sample Solution
 * 
 * File:   FrameAllocator.cpp
 * Author: Mike Goss <mikegoss@cs.du.edu>
 */

#include "FrameAllocator.h"

#include <cstring>
#include <sstream>
#include <stdexcept>

FrameAllocator::FrameAllocator(uint32_t page_frame_count, 
                               mem::MMU& mmu) 
: memory(page_frame_count * kPageSize, 0)
{
  if (page_frame_count <= 1) {
    throw std::runtime_error("page_frame_count must be > 1");
  }
  
  // Set free list empty
  uint32_t free_list_head = kEndList;
  
  // Add all page frames except 0 to free list
  uint32_t frame = memory.size() - kPageSize;
  
  while (frame > 0) {
    mmu.movb(&memory[frame], free_list_head, sizeof(uint32_t));
    free_list_head = frame;
    frame -= kPageSize;
  }
  
  // Initialize list info in page 0
  set_free_list_head(free_list_head);
  set_page_frames_free(page_frame_count - 1);
  set_page_frames_total(page_frame_count);
}

bool FrameAllocator::Allocate(uint32_t count, 
                              std::vector<uint32_t> &page_frames,
                              mem::MMU& mmu) {
  // Fetch free list info
  uint32_t page_frames_free = get_available();
  uint32_t free_list_head = get_free_list_head();
  
  // If enough pages available, allocate to caller
  if (count <= page_frames_free) {  // if enough to allocate
    while (count-- > 0) {
      // Return next free frame to caller
      uint32_t frame = free_list_head;
      page_frames.push_back(frame);
      
      // De-link frame from head of free list
      mmu.movb(free_list_head, &memory[free_list_head], sizeof(uint32_t));
      --page_frames_free;
      
      // Clear page frame to all 0
      mmu.movb(&memory[frame], 0, kPageSize);
    }
    
    // Update free list info
    set_free_list_head(free_list_head);
    set_page_frames_free(page_frames_free);

    return true;
  } else {
    return false;  // do nothing and return error
  }
}

bool FrameAllocator::Release(uint32_t count,
                             std::vector<uint32_t> &page_frames, 
                             mem::MMU& mmu) {
  // Fetch free list info
  uint32_t page_frames_free = get_available();
  uint32_t free_list_head = get_free_list_head();

  // If enough to deallocate
  if(count <= page_frames.size()) {
    while(count-- > 0) {
      // Return next frame to head of free list
      uint32_t frame = page_frames.back();
      page_frames.pop_back();
      mmu.movb(&memory[frame], free_list_head, sizeof(uint32_t));
      free_list_head = frame;
      ++page_frames_free;
    }

    // Update free list info
    set_free_list_head(free_list_head);
    set_page_frames_free(page_frames_free);

    return true;
  } else {
    return false; // do nothing and return error
  }
}

std::string FrameAllocator::get_available_list_string(void) const {
  std::ostringstream out_string;
  
  uint32_t next_free = get_free_list_head();
  
  while (next_free != kEndList) {
    out_string << " " << std::hex << next_free;
    memcpy(&next_free, &memory[next_free], sizeof(uint32_t));
  }
  
  return out_string.str();
}

uint32_t FrameAllocator::get_available() const {
  uint32_t page_frames_free;
  memcpy(&page_frames_free, &memory[kPageFramesFree], sizeof(uint32_t));
  return page_frames_free;
}

uint32_t FrameAllocator::get_frames_total() const {
  uint32_t page_frames_total;
  memcpy(&page_frames_total, &memory[kPageFramesTotal], sizeof(uint32_t));
  return page_frames_total;
}

uint32_t FrameAllocator::get_free_list_head() const {
  uint32_t free_list_head;
  memcpy(&free_list_head, &memory[kFreeListHead], sizeof(uint32_t));
  return free_list_head;
}

void FrameAllocator::set_page_frames_free(uint32_t page_frames_free) {
  memcpy(&memory[kPageFramesFree], &page_frames_free, sizeof(uint32_t));
}

void FrameAllocator::set_page_frames_total(uint32_t page_frames_total) {
  memcpy(&memory[kPageFramesTotal], &page_frames_total, sizeof(uint32_t));
}

void FrameAllocator::set_free_list_head(uint32_t free_list_head) {
  memcpy(&memory[kFreeListHead], &free_list_head, sizeof(uint32_t));
}
