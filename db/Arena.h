//
// Created by 王 瑞 on 2016/10/30.
//

#ifndef RELEVELDB_ARENA_H
#define RELEVELDB_ARENA_H

#include <cstddef>
#include <vector>
#include <assert.h>
#include <iostream>

#include "atomic_pointer.h"

namespace releveldb {

class Arena {
public:
  // No copying allowed
  Arena(const Arena&) = delete;
  Arena &operator=(const Arena&) = delete;

  Arena();
  ~Arena();

  // return a pointer newly allocated memory block with required bytes
  char* Allocate(size_t bytes);

  char* AllocateAligned(size_t bytes);

  size_t MemoryUsage() const {
    return memory_usage_.load();
  }

private:
  char* AllocateNewBlock(size_t bytes);
  char* AllocateFallback(size_t bytes);

  // Allocate State
  char* alloc_ptr_;
  size_t alloc_bytes_remaining_;

  // Array of new[] allocated memory blocks
  std::vector<char*> blocks_;

//  AtomicPointer memory_usage_;
  std::atomic<size_t> memory_usage_;
};

inline char* Arena::Allocate(size_t bytes) {
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaining_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }
  // else allocate memory.
  return AllocateFallback(bytes);
}

inline char* Arena::AllocateAligned(size_t bytes) {
  const int align = (sizeof(void*) > 8) ? sizeof(void*) : 8;
  assert((align & (align-1)) == 0);   // Pointer size should be a power of 2
  size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align-1);
  size_t slop = (current_mod == 0 ? 0 : align - current_mod);
  size_t needed = bytes + slop;
  char* result;
  if (needed <= alloc_bytes_remaining_) {
    result = alloc_ptr_ + slop;
    alloc_ptr_ += needed;
    alloc_bytes_remaining_ -= needed;
  } else {
    // AllocateFallback always returned aligned memory
    result = AllocateFallback(bytes);
  }
  assert((reinterpret_cast<uintptr_t>(result) & (align-1)) == 0);
  return result;
}

}


#endif //RELEVELDB_ARENA_H
