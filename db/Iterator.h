//
// Created by 王 瑞 on 2016/11/24.
//

#ifndef RELEVELDB_ITERATOR_H
#define RELEVELDB_ITERATOR_H

#include "db/Slice.h"
#include "db/Status.h"

namespace releveldb {

class Iterator {
public:
  Iterator(const Iterator&) = delete;
  void operator=(const Iterator&) = delete;

  Iterator();

  virtual ~Iterator();

  virtual bool Valid() const = 0;

  virtual void SeekToFirst() = 0;

  virtual void SeekToLast() = 0;

  virtual void Seek(const Slice &key) = 0;

  virtual void Next() = 0;

  virtual void Prev() = 0;

  virtual Slice Key() const = 0;

  virtual Slice Value() const = 0;

  virtual Status GetStatus() const = 0;

  typedef void (*CleanupFunction)(void* arg1, void* arg2);
  void RegisterCleanup(CleanupFunction function, void* arg1, void* arg2);

private:
  struct Cleanup {
    CleanupFunction function;
    void* arg1;
    void* arg2;
    Cleanup* next;
  };
  Cleanup cleanup_;
};

extern Iterator* NewEmptyIterator();

extern Iterator* NewErrorIterator(const Status& status);

}  // namespace releveldb


#endif //RELEVELDB_ITERATOR_H
