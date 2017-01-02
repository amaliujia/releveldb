//
// Created by 王 瑞 on 2017/1/1.
//

#include "db/Comparator.h"
#include "db/port_posix.h"

namespace releveldb {

Comparator::~Comparator() { }

namespace {

class BytewiseComparatorImpl : public Comparator {
public:
  BytewiseComparatorImpl() {}

  int Compare(const Slice& a, const Slice& b) const {
    return a.Compare(b);
  }

  // Used to check if comparator matches.
  const char* Name() const {
    return "releveldb.BytewiseComparator";
  }
};


}  // namespace

static OnceType once = PTHREAD_ONCE_INIT;
static const Comparator* bytewise;

static void InitModule() {
  bytewise = new BytewiseComparatorImpl();
}

const Comparator* BytewiseComparator() {
  InitOnce(&once, InitModule);
  return bytewise;
}

}  // namespace releveldb