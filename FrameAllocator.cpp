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
//: memory(page_frame_count * kPageSize, 0)
{
  if (page_frame_count <= 1) {
    throw std::runtime_error("page_frame_count must be > 1");
  }
  
  // Set free list empty
  uint32_t free_list_head = kEndList;
  
  // Add all page frames except 0 to free list
  uint32_t frame = kEndList - kPageSize;
  mem::Addr memFrame = (page_frame_count * 0x4000) - kPageSize;
  int i = 0;
  
  while (memFrame > 0) {
    //uint8_t buffer[] = {(uint8_t)(frame >> 24), (uint8_t)(frame >> 16), (uint8_t)(frame >> 8), (uint8_t)frame};
    //memcpy(&memory[frame], &free_list_head, sizeof(uint32_t));
    mem::Addr size32 = sizeof(uint32_t);
    mmu.movb(memFrame, &free_list_head, size32); 
          
    //mmu.movb(&memory[frame], free_list_head, sizeof(uint32_t));
    free_list_head = memFrame;
    memFrame -= kPageSize;
    
    i++;
    if (i == 127)
    {
        i = 127;
    }
  }
  
  // Initialize list info in page 0
  set_free_list_head(free_list_head, mmu);
  set_page_frames_free(page_frame_count - 1, mmu);
  set_page_frames_total(page_frame_count, mmu);
}

bool FrameAllocator::Allocate(uint32_t count, 
                            std::vector<uint32_t> &page_frames,
                            mem::MMU& mmu, 
                            mem::Addr vaddr) {
  // Fetch free list info
  uint32_t page_frames_free = get_available(mmu);
  uint32_t free_list_head = get_free_list_head(mmu);
  
  // If enough pages available, allocate to caller
  if (count <= page_frames_free) {  // if enough to allocate
    while (count-- > 0) {
      // Return next free frame to caller
      uint32_t frame = free_list_head;
      page_frames.push_back(frame);
      
      // De-link frame from head of free list
      mem::Addr size32 = sizeof(uint32_t);
      mem::Addr startFrame;
      
      if (vaddr == 0xFFFFFFF) {
        // De-link frame from head of free list
        startFrame = frame;
      
        mmu.movb(startFrame, &free_list_head, size32);
      }
      else {
          startFrame = vaddr;
          mmu.movb(vaddr, &free_list_head, size32);
      }
      
      --page_frames_free;
      
      // Clear page frame to all 0
      uint8_t buffer[] = {0};
      for (int i = 0; i < kPageSize; i++)
      {
          mmu.movb(startFrame+i, buffer, sizeof(buffer));
      }
    }
    
    // Update free list info
    set_free_list_head(free_list_head, mmu);
    set_page_frames_free(page_frames_free, mmu);

    return true;
  } else {
    return false;  // do nothing and return error
  }
}

bool FrameAllocator::Release(uint32_t count,
                             std::vector<uint32_t> &page_frames, 
                             mem::MMU& mmu) {
  // Fetch free list info
  uint32_t page_frames_free = get_available(mmu);
  uint32_t free_list_head = get_free_list_head(mmu);

  // If enough to deallocate
  if(count <= page_frames.size()) {
    while(count-- > 0) {
      // Return next frame to head of free list
      uint32_t frame = page_frames.back();
      page_frames.pop_back();
      mmu.movb(frame, &free_list_head, sizeof(uint32_t));
      free_list_head = frame;
      ++page_frames_free;
    }

    // Update free list info
    set_free_list_head(free_list_head, mmu);
    set_page_frames_free(page_frames_free, mmu);

    return true;
  } else {
    return false; // do nothing and return error
  }
}

std::string FrameAllocator::get_available_list_string(mem::MMU &mmu) const {
  std::ostringstream out_string;
  
  uint32_t next_free = get_free_list_head(mmu);
  
  while (next_free != kEndList) {
    out_string << " " << std::hex << next_free;
    mem::Addr ad = next_free;
    mmu.movb(&next_free, ad, sizeof(uint32_t));
  }
  
  return out_string.str();
}

uint32_t FrameAllocator::get_available(mem::MMU &mmu) const {
  uint32_t page_frames_free;
  mem::Addr ad = kPageFramesFree;
  mmu.movb(&page_frames_free, ad, sizeof(uint32_t));
  return page_frames_free;
}

uint32_t FrameAllocator::get_frames_total(mem::MMU &mmu) const {
  uint32_t page_frames_total;
  mem::Addr ad = kPageFramesTotal;
  mmu.movb(&page_frames_total, ad, sizeof(uint32_t));
  return page_frames_total;
}

uint32_t FrameAllocator::get_free_list_head(mem::MMU &mmu) const {
  uint32_t free_list_head;
  mem::Addr ad = kFreeListHead;
  mmu.movb(&free_list_head, ad, sizeof(uint32_t));
  return free_list_head;
}

void FrameAllocator::set_page_frames_free(uint32_t page_frames_free, mem::MMU &mmu) {
  mem::Addr ad = kPageFramesFree;
  mmu.movb(ad, &page_frames_free, sizeof(uint32_t));
}

void FrameAllocator::set_page_frames_total(uint32_t page_frames_total, mem::MMU &mmu) {
  mem::Addr ad = kPageFramesTotal;
  mmu.movb(ad, &page_frames_total, sizeof(uint32_t));
}

void FrameAllocator::set_free_list_head(uint32_t free_list_head, mem::MMU &mmu) {
  mem::Addr ad = kFreeListHead;
  mmu.movb(ad, &free_list_head, sizeof(uint32_t));
}
