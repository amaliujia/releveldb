//
// Created by SkyDragon on 2017/1/1.
//

#ifndef RELEVELDB_COMPARATOR_H
#define RELEVELDB_COMPARATOR_H

#include "db/Slice.h"

namespace releveldb {

class Slice;

// A comparator interface that provides a total order of all Slices.
class Comparator {
public:
  virtual ~Comparator();

  // Three way comparisons.
  //  return <0 if a < b
  //  return ==0 if a == b
  //  return >0 if a > b
  virtual int Compare(const Slice& a, const Slice& b) const = 0;

  // Used to check if comparator matches.
  virtual const char* Name() const = 0;

};

extern const Comparator* BytewiseComparator();

}  // namespace releveldb

#endif //RELEVELDB_COMPARATOR_H
