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

  // Advanced functions: these are used to reduce the space requirements
  // for internal data structures like index blocks.

  // If *start < limit, changes *start to a short string in [start,limit).
  // Simple comparator implementations may return with *start unchanged,
  // i.e., an implementation of this method that does nothing is correct.
  virtual void FindShortestSeparator(
  std::string* start,
  const Slice& limit) const = 0;

  // Changes *key to a short string >= *key.
  // Simple comparator implementations may return with *key unchanged,
  // i.e., an implementation of this method that does nothing is correct.
  virtual void FindShortSuccessor(std::string* key) const = 0;
};

extern const Comparator* BytewiseComparator();

}  // namespace releveldb

#endif //RELEVELDB_COMPARATOR_H
