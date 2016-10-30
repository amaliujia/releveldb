//
// Created by 王 瑞 on 2016/10/29.
//

#include "Slice.h"

namespace releveldb {

// -1, *this < slice
// 0, *this == slice
// 1, *this > slice
int Slice::Compare(const Slice& slice) const {
  const size_t minLen = (Size() < slice.Size()) ? Size() : slice.Size();
  int cmp = memcmp(Data(), slice.Data(), minLen);
  if (cmp == 0) {
    if (Size() < slice.Size()) {
      cmp = -1;
    } else if (Size() > slice.Size()) {
      cmp = 1;
    }
  }
  return cmp;
}

inline bool Slice::operator==(const Slice& slice) {
  return (Size() == slice.Size()) && memcmp(Data(), slice.Data(), Size());
}

inline bool Slice::operator!=(const Slice &slice) {
  return !(*this == slice);
}

}  // releveldb