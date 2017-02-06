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
#include "db/atomic_pointer.h"

namespace releveldb {

class Arena;

template<typename Key, class Comparator>
class SkipList {
public:
  explicit SkipList(Comparator cmo, Arena* arena);
  SkipList() = delete;

  // Insert key into the list.
  // REQUIRES: nothing that compares equal to key is currently in the list.
  void Insert(const Key& key);

  // Returns true iff an entry that compares equal to key is in the list.
  bool Contains(const Key& key) const;

private:
  enum {
    kMaxHeight = 12
  };

  // Immutable after construction
  Comparator const compare_;
  Arena* const arena_;    // Arena used for allocations of nodes

  Node* const head_;

  AtomicPointer max_height_;

  Random rand_;

  inline int GetMaxHeight() const {
    return reinterpret_cast<intptr_t>(max_height_.NoBarrier_Load());
  }

  Node* NewNode(const Key& key, int height);

private:
  struct Node;
};

template <typename Key, class Comparator>
Node* SkipList<Key,Comparator>::NewNode(const Key& key, int height) {
  char* mem = arena_->AllocateAligned(sizeof(Node) +
                                      sizeof(AtomicPointer) * (height - 1));
  return new (mem) Node(key);

};


template <typename Key, class Comparator>
SkipList<Key, Comparator>::SkipList(Comparator comp, Arena* arena):
    compare_(comp), arena_(arena),
    // Head node does not matter. But do we need
    // tail node?
    head_(NewNode(0 /*random key */, kMaxHeight)),
    max_height_(reinterpret_cast<void*>(1)),
    rand_(0xdeadbeef)
{
  // I guess array in head_ is head array, each one is a start of
  // linked list.
  for (int i = 0; i < kMaxHeight; i++) {
    head_->SetNext(i, NULL);
  }
}

template<typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {
public:
  Key const key;

  explicit Node(const Key& k) : key(k) { }

  // Accessors/mutators for links.  Wrapped in methods so we can
  // add the appropriate barriers as necessary.
  Node* Next(int n) {
    assert(n >= 0);
    // Use an 'acquire load' so that we observe a fully initialized
    // version of the returned Node.
    return reinterpret_cast<Node*>(next_[n].Acquire_Load());
  }
  void SetNext(int n, Node* x) {
    assert(n >= 0);
    // Use a 'release store' so that anybody who reads through this
    // pointer observes a fully initialized version of the inserted node.
    next_[n].Release_Store(x);
  }

  // No-barrier variants that can be safely used in a few locations.
  Node* NoBarrier_Next(int n) {
    assert(n >= 0);
    return reinterpret_cast<Node*>(next_[n].NoBarrier_Load());
  }
  void NoBarrier_SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].NoBarrier_Store(x);
  }

private:
  AtomicPointer next_[1];
};

}  // namespace releveldb

#endif //RELEVELDB_SKIPLIST_H
