//
// Created by 王 瑞 on 2017/1/15.
//

#ifndef RELEVELDB_SKIPLIST_H
#define RELEVELDB_SKIPLIST_H

#include <assert.h>
#include <stdlib.h>
#include "db/port_posix.h"
#include "db/Arena.h"
#include "db/random.h"

namespace releveldb {

class Arena;

template<typename Key, class Comparator>
class SkipList {
public:
  explicit SkipList(Comparator cmo, Arena* arena);
  SkipList() = delete;
};

}  // namespace releveldb

#endif //RELEVELDB_SKIPLIST_H
