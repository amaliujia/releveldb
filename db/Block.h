//
// Created by rui_wang on 1/2/17.
//

#ifndef RELEVELDB_BLOCK_H
#define RELEVELDB_BLOCK_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include "db/Iterator.h"
#include "db/Comparator.h"
#include "db/Format.h"
#include "db/Slice.h"
#include "db/Status.h"

namespace releveldb {

struct BlockContents;
class Comparator;

class Block {
public:
  explicit Block(const BlockContents& content);

  ~Block();

  size_t Size() const {
    return size_;
  }

  Iterator* NewIterator(const Comparator* comparator);

private:
  inline uint32_t NumRestarts() const;

private:
  const char* data_;
  size_t size_;
  uint32_t restart_offset_;     // Offset in data_ of restart array
  bool owned_;                  // Block owns data_[]

  class Iter;
};

}
#endif //RELEVELDB_BLOCK_H
