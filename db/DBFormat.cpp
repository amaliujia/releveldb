//
// Created by 王 瑞 on 2016/11/27.
//


#include "DBFormat.h"

namespace releveldb {

static uint64_t PackSequenceAndType(uint64_t seq, ValueType t) {
  assert(seq <= kMaxSequenceNumber);
  assert(t <= kValueTypeForSeek);
  return (seq << 8) | t;
}

void AppendInternalKey(std::string* result,
                       const ParsedInternalKey& key) {
  result->append(key.user_key.Data(), key.user_key.Size());
  PutFixed64(result, PackSequenceAndType(key.sequence, key.type));
}

inline bool ParseInternalKey(const Slice& internal_key,
                        ParsedInternalKey* result) {
  const size_t size = internal_key.Size();
  if (size < 8) {
    return false;
  }

  uint64_t num = DecodeFixed64(internal_key.Data());
  unsigned char c = num & 0xff;
  result->sequence = num >> 8;
  result->type = static_cast<ValueType>(c);
  result->user_key = Slice(internal_key.Data(), internal_key.Size() - 8);
  return true;
}

std::string ParsedInternalKey::DebugString() const {
  char buf[50];
  snprintf(buf, sizeof(buf), "@ %llu : %d",
           (unsigned long long)sequence,
           int(type));

  std::string result;
  result.push_back('\'');
  result.append(EscapeString(user_key));
  result.append(buf);
  return result;
}

const char* InternalKeyComparator::Name() const {
  return "releveldb.InternalKeyComparator";
}

// Order by:
//    increasing user key (according to user-supplied comparator)
//    decreasing sequence number
//    decreasing type (though sequence# should be enough to disambiguate)
int InternalKeyComparator::Compare(const Slice& a, const Slice& b) const {
  int result = user_comparator_->Compare(ExtractUserKey(a), ExtractUserKey(b));
  if (result == 0) {
    uint64_t num_a = DecodeFixed64(a.Data() + a.Size() - 8);
    uint64_t num_b = DecodeFixed64(b.Data() + b.Size() - 8);
    if (num_a > num_b) {
      result = -1;
    } else if (num_a < num_b) {
      result = +1;
    }
  }
  return result;
}

inline int InternalKeyComparator::Compare(const InternalKey& a, const InternalKey& b) const {
  return Compare(a.Encode(), b.Encode());
}

std::string InternalKey::DebugString() const {
  std::string result;
  ParsedInternalKey parsed;
  if (ParseInternalKey(rep_, &parsed)) {
    result = parsed.DebugString();
  } else {
    result = "(bad)";
    result.append(EscapeString(rep_));
  }
  return result;
}

}  // namespace releveldb