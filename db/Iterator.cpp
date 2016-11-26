//
// Created by 王 瑞 on 2016/11/24.
//

#include "Iterator.h"

namespace releveldb {

Iterator::Iterator() {
  cleanup_.function = nullptr;
  cleanup_.next = nullptr;
  cleanup_.arg1 = nullptr;
  cleanup_.arg2 = nullptr;
}

Iterator::~Iterator() {
  if (cleanup_.function != nullptr) {
    (*cleanup_.function)(cleanup_.arg1, cleanup_.arg2);
    for (Cleanup* c = cleanup_.next; c != nullptr; ) {
      (*c->function)(c->arg1, c->arg2);
      Cleanup* next = c->next;
      delete c;
      c = next;
    }
  }
}

void Iterator::RegisterCleanup(CleanupFunction function, void *arg1, void *arg2) {
  assert(function != nullptr);

  Cleanup* c;
  if (cleanup_.function == nullptr) {
    c = &cleanup_;
  } else {
    c = new Cleanup;
    c->next = cleanup_.next;
    cleanup_.next = c;
  }

  c->function = function;
  c->arg1 = arg1;
  c->arg2 = arg2;
}

namespace {
class EmptyIterator : public Iterator {
public:
  EmptyIterator(const Status& s):status_(s) { }
  ~EmptyIterator() { }
  virtual bool Valid() const { return false; }
  virtual void Seek(const Slice& key) { }
  virtual void SeekToFirst() { }
  virtual void SeekToLast() { }
  // cannot be called
  virtual void Next() { assert(false); }
  virtual void Prev() { assert(false); }
  virtual Slice Key() const { assert(false); return Slice(); }
  virtual Slice Value() const { assert(false); return Slice(); }
  virtual Status GetStatus() const { return status_; }
private:
  Status status_;
};
}  // namespace

Iterator* NewEmptyIterator() {
  return new EmptyIterator(Status::OK());
}

Iterator *NewErrorIterator(const Status& status) {
  return new EmptyIterator(status);
}

}  // namespace releveldb
