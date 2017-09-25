//
// Created by 王 瑞 on 2017/6/18.
//

#include <gtest/gtest.h>
#include "db/SkipList.h"
#include "db/Arena.h"


namespace releveldb {

class SkipListTests : public ::testing::Test {
};

typedef uint64_t Key;

struct Comparator {
  int operator()(const Key &a, const Key &b) const {
    if (a < b) {
      return -1;
    } else if (a > b) {
      return +1;
    } else {
      return 0;
    }
  }
};

TEST(SkipTest, EmptyTest) {
  Arena arena;
  Comparator cmp;
  SkipList<Key, Comparator> list(cmp, &arena);
  ASSERT_TRUE(!list.Contains(10));

//  SkipList<Key, Comparator>::Iterator iter(&list);
//  ASSERT_TRUE(!iter.Valid());
//  iter.SeekToFirst();
//  ASSERT_TRUE(!iter.Valid());
//  iter.Seek(100);
//  ASSERT_TRUE(!iter.Valid());
//  iter.SeekToLast();
//  ASSERT_TRUE(!iter.Valid());
}

}