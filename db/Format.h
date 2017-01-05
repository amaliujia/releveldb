//
// Created by rui_wang on 1/2/17.
//

#ifndef RELEVELDB_FORMAT_H
#define RELEVELDB_FORMAT_H

#include "db/Slice.h"
#include "db/Status.h"

namespace releveldb {

struct BlockContents {
  Slice data;           // Actual contents of data
  bool cachable;        // True iff data can be cached
  bool heap_allocated;  // True iff caller should delete[] data.data()
};

}

#endif //RELEVELDB_FORMAT_H
