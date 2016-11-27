// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <mutex>


#ifndef STORAGE_LEVELDB_UTIL_MUTEXLOCK_H_
#define STORAGE_LEVELDB_UTIL_MUTEXLOCK_H_

namespace releveldb {

// Helper class that locks a mutex on construction and unlocks the mutex when
// the destructor of the MutexLock object is invoked.
//
// Typical usage:
//
//   void MyClass::MyMethod() {
//     MutexLock l(&mu_);       // mu_ is an instance variable
//     ... some complex code, possibly with multiple return paths ...
//   }

class  MutexLock {
public:
explicit MutexLock(std::mutex *mu)
: mu_(mu)  {
  this->mu_->lock();
}
~MutexLock()  { this->mu_->unlock(); }

private:
std::mutex* const mu_;
// No copying allowed
MutexLock(const MutexLock&);
void operator=(const MutexLock&);
};

}  // namespace releveldb


#endif  // STORAGE_LEVELDB_UTIL_MUTEXLOCK_H_
