/* FrameAllocator - allocate pages in memory
 * 
 * COMP3361 Winter 2019 - Lab 3 Sample Solution
 * 
 * File:   FrameAllocator.h
 * Author: Mike Goss <mikegoss@cs.du.edu>
 */

#ifndef FRAMEALLOCATOR_H
#define FRAMEALLOCATOR_H

#include <cstdint>
#include <string>
#include <vector>

class FrameAllocator {
public:
  /**
   * Constructor
   * 
   * Allocates the specified number of page frames, and builds free list of
   * all page frames.
   * 
   * @param page_frame_count
   */
  FrameAllocator(uint32_t page_frame_count);
  
  virtual ~FrameAllocator() {}  // empty destructor
  
  // Disallow copy/move
  FrameAllocator(const FrameAllocator &other) = delete;
  FrameAllocator(FrameAllocator &&other) = delete;
  FrameAllocator &operator=(const FrameAllocator &other) = delete;
  FrameAllocator &operator=(FrameAllocator &&other) = delete;
  
  /**
   * Allocate - allocate page frames from the free list
   * 
   * @param count number of page frames to allocate
   * @param page_frames page frame addresses allocated are pushed on back
   * @return true if success, false if insufficient page frames (no frames allocated)
   */
  bool Allocate(uint32_t count, std::vector<uint32_t> &page_frames);
  
  /**
   * Release - return page frames to free list
   * 
   * @param count number of page frames to free
   * @param page_frames contains page frame addresses to deallocate; addresses
   *   are popped from back of vector
   * @return true if success, false if insufficient page frames in vector
   */
  bool Release(uint32_t count, std::vector<uint32_t> &page_frames);
  
  // Functions to return list info
  uint32_t get_available(void) const;
  uint32_t get_frames_total(void) const;
  
  /**
   * get_available_list_string - get string representation of free list
   * 
   * @return hex numbers of all free pages
   */
  std::string get_available_list_string(void) const;
  
  static const uint32_t kPageSize = 0x4000;
private:
  // Vector to hold memory to be allocated
  std::vector<uint8_t> memory;
  
  // Address of number of first free page frame
  static const size_t kFreeListHead = 0;
  
  // Total number of page frames
  static const size_t kPageFramesTotal = kFreeListHead + sizeof(uint32_t);
  
  // Current number of free page frames
  static const size_t kPageFramesFree = kPageFramesTotal + sizeof(uint32_t);
  
  // End of list marker
  static const uint32_t kEndList = 0xFFFFFFFF;
  
  // Private getters and setters
  uint32_t get_free_list_head(void) const;
  void set_free_list_head(uint32_t free_list_head);
  void set_page_frames_total(uint32_t page_frames_total);
  void set_page_frames_free(uint32_t page_frames_free);
};

#endif /* FRAMEALLOCATOR_H */

