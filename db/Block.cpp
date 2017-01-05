//
// Created by rui_wang on 1/2/17.
//

#include "Block.h"
#include "db/Coding.h"

namespace releveldb {

uint32_t Block::NumRestarts() const {
  assert(size_ >= sizeof(uint32_t));
  return DecodeFixed32(data_ + size_ - sizeof(uint32_t));
}

Block::Block(const BlockContents& content): data_(content.data.Data()),
                                            size_(content.data.Size()),
                                            owned_(content.heap_allocated)
{
  if (size_ < sizeof(uint32_t)) {
    size_ = 0;  // error marker, 0
  } else {
    size_t max_num_restart = (size_ - sizeof(uint32_t)) / sizeof(uint32_t);
    if (NumRestarts() > max_num_restart) {
      // The size is too small for NumRestarts()
      size_ = 0;
    } else {
      restart_offset_ = size_ - (1 + NumRestarts()) * sizeof(uint32_t);
    }
  }
}

Block::~Block() {
  if (owned_) {
    delete[] data_;
    // should set size_ as 0 and heap_allocated_ as false?
  }
}

class Block::Iter : public Iterator {
public:
  Iter(const Comparator* comparator,
        const char* const data,
        uint32_t const restarts,
        uint32_t const num_restarts):
        comparator_(comparator),
        data_(data),
        restarts_(restarts),
        num_restarts_(num_restarts) {
          assert(num_restarts_ > 0);
  }

  virtual bool Valid() const {
    return current_ < restarts_;
  }

  virtual void SeekToFirst() = 0;

  virtual void SeekToLast() = 0;

  virtual void Seek(const Slice &key) = 0;

  virtual void Next() = 0;

  virtual void Prev() = 0;

  virtual Slice Key() const {
    assert(Valid());
    return key_;
  }

  virtual Slice Value() const {
    assert(Valid());
    return value_;
  }

  virtual Status GetStatus() const {
    return status_;
  };

private:
  const Comparator* const comparator_;
  const char* const data_;      // underlying block contents
  uint32_t const restarts_;     // Offset of restart array (list of fixed32)
  uint32_t const num_restarts_; // Number of uint32_t entries in restart array

  // current_ is offset in data_ of current entry.  >= restarts_ if !Valid
  uint32_t current_;
  uint32_t restart_index_;  // Index of restart block in which current_ falls
  std::string key_;
  Slice value_;
  Status status_;
};

}