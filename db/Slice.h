//
// Created by amaliuji on 2016/10/29.
//

#ifndef RELEVELDB_SLICE_H
#define RELEVELDB_SLICE_H

#include <cstdio>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace releveldb {

class Slice {
public:
  //  Slice(const Slice& ) = delete;
  Slice& operator=(const Slice&) = delete;

  Slice(): data_(""), size_(0) { }
  Slice(const char* data, size_t size): data_(data),
                                        size_(size) { }
  Slice(const std::string& dataStr): data_(dataStr.data()),
                                     size_(dataStr.size()) {}
  Slice(const char* data): data_(data), size_(strlen(data)) {}

  ~Slice() {
    Clear();
  }

  inline const char* Data() const {
    return data_;
  }

  inline size_t Size() const {
    return size_;
  }

  bool Empty() const {
    return size_ == 0;
  }

  char operator[](size_t i) const;

  void Clear() {
    data_ = "";
    size_ = 0;
  }

  void DropFirstNBytes(size_t n) {
    assert(n <= Size());
    data_ += n;
    size_ -= n;
  }

  std::string String() const {
    return std::string(data_, size_);
  }

  int Compare(const Slice& slice) const;

private:
  const char* data_;
  size_t size_;

};

inline bool operator==(const Slice& x, const Slice& y) {
  return (x.Size() == y.Size()) && (memcmp(x.Data(), y.Data(), x.Size()) == 0);
}

inline bool operator!=(const Slice& x, const Slice& y) {
  return !(x == y);
}

}  // releveldb

#endif //RELEVELDB_SLICE_H
