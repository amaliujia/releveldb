//
// Created by rui_wang on 1/2/17.
//

#ifndef RELEVELDB_DB_H
#define RELEVELDB_DB_H

#include <stdint.h>
#include <stdio.h>

#include "db/Options.h"
#include "db/Iterator.h"

namespace releveldb {

// Update Makefile if you change these
static const int kMajorVersion = 1;
static const int kMinorVersion = 19;

struct ReadOptions;

// Abstract handle to particular state of a DB.
// A Snapshot is an immutable object and can therefore be safely
// accessed from multiple threads without any external synchronization.
class Snapshot {
protected:
  virtual ~Snapshot();
};

}  // namespace releveldb

#endif //RELEVELDB_DB_H
