//
// Created by 王 瑞 on 2016/10/30.
//

#include "Arena.h"

#include <assert.h>

namespace releveldb {

static const int kBlockSize = 4096;

Arena::Arena() {
  alloc_ptr_ = nullptr;
  alloc_bytes_remaining_ = 0;
}

char* Arena::Allocate(size_t bytes) {
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaining_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }

  // else allocate memory.
}

}