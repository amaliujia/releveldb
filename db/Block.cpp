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

// Helper routine: decode the next block entry starting at "p",
// storing the number of shared key bytes, non_shared key bytes,
// and the length of the value in "*shared", "*non_shared", and
// "*value_length", respectively.  Will not dereference past "limit".
//
// If any errors are detected, returns NULL.  Otherwise, returns a
// pointer to the key delta (just past the three decoded values).
static inline const char* DecodeEntry(const char* p, const char* limit,
                                      uint32_t* shared,
                                      uint32_t* non_shared,
                                      uint32_t* value_length) {
  // limit is the bound of p, cannot dereferene past this bound.
  if (limit - p < 3) return NULL;
  // Three bytes save 3 int.
  *shared = reinterpret_cast<const unsigned char*>(p)[0];
  *non_shared = reinterpret_cast<const unsigned char*>(p)[1];
  *value_length = reinterpret_cast<const unsigned char*>(p)[2];
  // the highest bit should be 0, so 01111111 < 128
  if ((*shared | *non_shared | *value_length) < 128) {
    // Fast path: all three values are encoded in one byte each
    p += 3;
  } else {
    // 分别解析每个长度: 共享key长度、非共享key长度、value长度
    if ((p = GetVarint32Ptr(p, limit, shared)) == NULL) return NULL;
    if ((p = GetVarint32Ptr(p, limit, non_shared)) == NULL) return NULL;
    if ((p = GetVarint32Ptr(p, limit, value_length)) == NULL) return NULL;
  }

  // non_shared + value_length is equal to left chars starting with p.
  // 剩余数据长度 < 解析的"key非共享内存"和"value内容"长度，则数据非法
  if (static_cast<uint32_t>(limit - p) < (*non_shared + *value_length)) {
    return NULL;
  }
  return p;
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

  virtual void Seek(const Slice &target) {
    // Binary search in restart array to find the last restart point
    // with a key < target
    uint32_t left = 0;
    uint32_t right = num_restarts_ - 1;

    while (left < right) {
      uint32_t mid = (left + right + 1) / 2;
      uint32_t region_offset = GetRestartPoint(mid);
      uint32_t shared, non_shared, value_length;

      const char* key_ptr = DecodeEntry(data_ + region_offset,
                                        data_ + restarts_,
                                        &shared, &non_shared, &value_length);

      if (key_ptr == NULL || (shared != 0)) {
        CorruptionError();
        return;
      }

      Slice mid_key(key_ptr, non_shared);
      if (Compare(mid_key, target) < 0) {
        // Key at "mid" is smaller than "target".  Therefore all
        // blocks before "mid" are uninteresting.
        left = mid;
      } else {
        // Key at "mid" is >= "target".  Therefore all blocks at or
        // after "mid" are uninteresting.
        right = mid - 1;
      }
    }

    SeekToRestartPoint(left);
    while (true) {
      if (!ParseNextKey()) {
        return;
      }
      if (Compare(key_, target) >= 0) {
        return;
      }
    }
  }

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
  void SeekToRestartPoint(uint32_t index) {
    key_.clear();
    restart_index_ = index;
    // current_ will be fixed by ParseNextKey();

    // ParseNextKey() starts at the end of value_, so set value_ accordingly
    uint32_t offset = GetRestartPoint(index);
    value_ = Slice(data_ + offset, 0);
  }

  bool ParseNextKey() {
    current_ = NextEntryOffset();
    const char* p = data_ + current_;
    const char* limit = data_ + restarts_;  // Restarts come right after data
    if (p >= limit) {
      // No more entries to return.  Mark as invalid.
      current_ = restarts_;
      restart_index_ = num_restarts_;
      return false;
    }

    // Decode next entry
    uint32_t shared, non_shared, value_length;
    p = DecodeEntry(p, limit, &shared, &non_shared, &value_length);
    if (p == NULL || key_.size() < shared) {
      CorruptionError();
      return false;
    } else {
      key_.resize(shared);
      key_.append(p, non_shared);
      value_ = Slice(p + non_shared, value_length);
      while (restart_index_ + 1 < num_restarts_ &&
             GetRestartPoint(restart_index_ + 1) < current_) {
        ++restart_index_;
      }
      return true;
    }
  }

  uint32_t GetRestartPoint(uint32_t index) {
    assert(index < num_restarts_);
    return DecodeFixed32(data_ + restarts_ + index * sizeof(uint32_t));
  }

  inline int Compare(const Slice& a, const Slice& b) const {
    return comparator_->Compare(a, b);
  }

  // Return the offset in data_ just past the end of the current entry.
  inline uint32_t NextEntryOffset() const {
    return (value_.Data() + value_.Size()) - data_;
  }

  void CorruptionError() {
    current_ = restarts_;
    restart_index_ = num_restarts_;
    status_ = Status::Corruption("bad entry in block");
    key_.clear();
    value_.Clear();
  }

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