//
// Created by 王 瑞 on 2016/11/27.
//

#ifndef RELEVELDB_DBFORMAT_H
#define RELEVELDB_DBFORMAT_H

#include <stdio.h>
#include <string>

#include "db/Common.h"
#include "db/Coding.h"
#include "db/Slice.h"
#include "db/Logging.h"
#include "db/Comparator.h"

namespace releveldb {

namespace {
static const int kNumLevels = 7;

// Level-0 compaction is started when we hit this many files.
static const int kL0_CompactionTrigger = 4;

// Soft limit on number of level-0 files.  We slow down writes at this point.
static const int kL0_SlowdownWritesTrigger = 8;

// Maximum number of level-0 files.  We stop writes at this point.
static const int kL0_StopWritesTrigger = 12;

// Maximum level to which a new compacted memtable is pushed if it
// does not create overlap.  We try to push to level 2 to avoid the
// relatively expensive level 0=>1 compactions and to avoid some
// expensive manifest file operations.  We do not push all the way to
// the largest level since that can generate a lot of wasted disk
// space if the same key space is being repeatedly overwritten.
static const int kMaxMemCompactLevel = 2;

// Approximate gap in bytes between samples of data read during iteration.
static const int kReadBytesPeriod = 1048576;

}  // namespace

class InternalKey;

// Value types encoded as the last component of internal keys.
// DO NOT CHANGE THESE ENUM VALUES: they are embedded in the on-disk
// data structures.
enum ValueType {
  kTypeDeletion = 0x0,
  kTypeValue = 0x1
};

static const ValueType kValueTypeForSeek = kTypeValue;

typedef uint64_t SequenceNumber;

// We leave eight bits empty at the bottom so a type and sequence#
// can be packed together into 64-bits.
static const SequenceNumber kMaxSequenceNumber = ((0x1ull << 56) - 1);

struct ParsedInternalKey {
  Slice user_key;
  SequenceNumber sequence;
  ValueType type;

  ParsedInternalKey() { } // Intentionally left uninitialized (for speed)

  ParsedInternalKey(const Slice& u, const SequenceNumber& seq, ValueType t)
  : user_key(u), sequence(seq), type(t) { }
  std::string DebugString() const;
};

// Return the length of the encoding of "key".
inline size_t InternalKeyEncodingLength(const ParsedInternalKey& key) {
  return key.user_key.Size() + 8;
}

// Append the serialization of "key" to *result.
extern void AppendInternalKey(std::string* result,
                              const ParsedInternalKey& key);

// Attempt to parse an internal key from "internal_key".  On success,
// stores the parsed data in "*result", and returns true.
//
// On error, returns false, leaves "*result" in an undefined state.
extern bool ParseInternalKey(const Slice& internal_key,
                             ParsedInternalKey* result);

// Returns the user key portion of an internal key.
inline Slice ExtractUserKey(const Slice& internal_key) {
  assert(internal_key.Size() >= 8);
  return Slice(internal_key.Data(), internal_key.Size() - 8);
}

inline ValueType ExtractValueType(const Slice& internal_key) {
  assert(internal_key.Size() >= 8);
  const size_t n = internal_key.Size();
  uint64_t num = DecodeFixed64(internal_key.Data() + n - 8);
  unsigned char c = num & 0xff;
  return static_cast<ValueType>(c);
}

// A comparator for internal keys that uses a specified comparator for
// the user key portion and breaks ties by decreasing sequence number.
class InternalKeyComparator : public Comparator {
private:
  const Comparator* user_comparator_;
public:
  explicit InternalKeyComparator(const Comparator* c) :
      user_comparator_(c) { }

  virtual const char* Name() const;

  virtual int Compare(const Slice& a, const Slice& b) const;

  const Comparator* user_comparator() const { return user_comparator_; }

  int Compare(const InternalKey& a, const InternalKey& b) const;
};

class InternalKey {
public:
  InternalKey() {}
  InternalKey(const Slice& key, SequenceNumber s, ValueType t) {
    AppendInternalKey(&rep_, ParsedInternalKey(key, s, t));
  }

  void DecodeFrom(const Slice& s) {
    rep_.assign(s.Data(), s.Size());
  }

  Slice Encode() const {
    assert(!rep_.empty());
    // Call a constructor and then do a copy?
    return Slice(rep_);
  }

  Slice UserKey() const {
    // Call a constructor and then do a copy?
    return ExtractUserKey(rep_);
  }

  void Clear() {
    rep_.clear();
  }

  std::string DebugString() const;

private:
  std::string rep_;
};

}  // namespace releveldb

#endif //RELEVELDB_DBFORMAT_H
