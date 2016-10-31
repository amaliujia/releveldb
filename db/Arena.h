//
// Created by 王 瑞 on 2016/10/30.
//

#ifndef RELEVELDB_ARENA_H
#define RELEVELDB_ARENA_H

#include <cstddef>
#include <vector>

#include "atomic_pointer.h"

namespace releveldb {

class Arena {
public:
  Arena();
  ~Arena();

  inline char* Allocate(size_t bytes);

private:
  // Allocate State
  char* alloc_ptr_;
  size_t alloc_bytes_remaining_;

  // Array of new[] allocated memory blocks
  std::vector<char*> blocks_;
};

}


#endif //RELEVELDB_ARENA_H
