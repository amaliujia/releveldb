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
private:
  struct Node;

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

  struct Node* const head_;

  AtomicPointer max_height_;

  Random rand_;

  inline int GetMaxHeight() const {
    return reinterpret_cast<intptr_t>(max_height_.NoBarrier_Load());
  }

  inline bool Equal(const Key& a, const Key& b) const {
    return (compare_(a, b) == 0);
  }

  // Returns a Node whose key is greater or equal to key, or returns NULL.
  // If prev is not NULL, it will be filled out with pointers to Nodes whose
  // keys are smaller than key in corresponding levels. prev has to pointing
  // to a Node* array with length kMaxHeight.
  Node* FindGreaterOrEqual(const Key& key, Node** prev) const;

  bool KeyIsAfterNode(const Key& key, Node* n) const;

  Node* NewNode(const Key& key, int height);
};

template<typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {
public:
  Key const key;
  explicit  Node(const Key& k): key(k) { }

  Node* Next(int n) {
    assert(n >= 0);
    return reinterpret_cast<Node*>(next_[n].Acquire_Load());
  }

  void SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].Release_Store(reinterpret_cast<void*>(x));
  }

  Node* NoBarrier_Next(int n) {
    assert(n >= 0);
    return reinterpret_cast<Node*>(next_[n].NoBarrier_Load());
  }

  void NoBarrier_SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].NoBarrier_Store(reinterpret_cast<void*>(x));
  }

private:
  AtomicPointer next_[1];
};

template <typename Key, class Comparator>
bool SkipList<Key, Comparator>::Contains(const Key &key) const {
  Node* x = FindGreaterOrEqual(key, NULL);
  if (x != NULL && Equal(key, x->key)) {
    return true;
  } else {
    return false;
  }
}

template <typename Key, class Comparator>
void SkipList<Key, Comparator>::Insert(const Key& key) {
  // Nodes* array to save accessors nodes.
  Node* prev[kMaxHeight];
  Node* x = FindGreaterOrEqual(key, prev);

  // Does not allow duplicate insertion so x->key != key if not NULL.
  assert(x == NULL || !Equal(x->key, key));

  //TODO: Should be random height
  int height = 5;
  if (height > GetMaxHeight()) {
    for (int i = GetMaxHeight(); i < height; i++) {
      prev[i] = head_;
    }

    max_height_.Release_Store(reinterpret_cast<void*>(height));
    //TODO: Do not need memory barrier
    //max_height_.NoBarrier_Store(reinterpret_cast<void*>(height));
  }

  x = NewNode(key, height);
  for (int i = 0; i < height; i++) {
    //TODO: Why no memory barrier here.
    x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
    prev[i]->SetNext(i, x);
  }
};

template <typename Key, class Comparator>
bool  SkipList<Key,Comparator>::KeyIsAfterNode(const Key &key, Node *n) const {
  // NULL pointer = infinite.
  // comp_(a, b) < 0 means a < b
  return (n != nullptr) && compare_(n->key, key) < 0;
}

template <typename Key, class Comparator>
typename SkipList<Key,Comparator>::Node* SkipList<Key,Comparator>::FindGreaterOrEqual(const Key &key,
                                                   Node **prev) const {
  // start with head nodes
  Node* x = head_;
  // start with current highest level
  int level = GetMaxHeight() - 1;

  // loop to set prev of nodes that right before before key (key greater
  // or equal to nodes)
  while (true) {
    Node* next = x->Next(level);
    if (KeyIsAfterNode(key, next)) {
      // Get in cause next->key < key.
      //
      // Keep search this list to find a node that >= key or
      // find the end of list, which means key should be inserted
      // between x and next.
      x = next;
    } else {
      if (prev != NULL) {
        // prev is allowed to be NULL.

        // if not NULL, prev[level] should be x cause x < key <= next.
        prev[level] = x;
      }

      if (level ==0) {
        // return next case now next->key >= key.
        return next;
      } else {
        // search next level.
        // Note current x is the node that < key
        // so lower level should start from x to search.
        level--;
      }
    }
  }
}

template <typename Key, class Comparator>
typename SkipList<Key,Comparator>::Node* SkipList<Key,Comparator>::NewNode(const Key& key, int height) {

  char* mem = arena_->AllocateAligned(sizeof(Node) +
                                      sizeof(AtomicPointer) * (height - 1));

  // Call Node constructor on mem (pre-allocated memory)
  return new (mem) Node(key);

};

template <typename Key, class Comparator>
SkipList<Key, Comparator>::SkipList(Comparator comp, Arena* arena):
    compare_(comp), arena_(arena),
    // Head node does not matter. But do we need
    // tail node?
    head_(NewNode(0 /*random key */, kMaxHeight)),
    // initialize height as 1
    max_height_(reinterpret_cast<void*>(1)),
    rand_(0xdeadbeef)
{
  // I guess array in head_ is head array, each one is a start of
  // linked list.
  for (int i = 0; i < kMaxHeight; i++) {
    head_->SetNext(i, NULL);
  }
}
}  // namespace releveldb

#endif //RELEVELDB_SKIPLIST_H
