//
// Created by 王 瑞 on 2016/10/30.
//

#include "Arena.h"

namespace releveldb {

static const int kBlockSize = 4096;

Arena::Arena():  memory_usage_(0) {
  alloc_ptr_ = nullptr;
  alloc_bytes_remaining_ = 0;
}

Arena::~Arena() {
  for (size_t i = 0; i < blocks_.size(); i++) {
    delete[] blocks_[i];
  }
}

char* Arena::AllocateFallback(size_t bytes) {
  if (bytes > kBlockSize / 4) {
    // if greater than one quarter of block size, Allocate it separately
    // to avoid wasting too much space in leftover bytes.
    char* ret = AllocateNewBlock(bytes);
    return ret;
  }

  alloc_ptr_ = AllocateNewBlock(kBlockSize);
  char* ret = alloc_ptr_;
  alloc_ptr_ += bytes;
  alloc_bytes_remaining_ = kBlockSize - bytes;
  return ret;
}

char* Arena::AllocateNewBlock(size_t bytes) {
  char* ret = new char[bytes];
  blocks_.push_back(ret);
  memory_usage_.store(memory_usage_.load() + bytes + sizeof(char*));
  return ret;
}

}