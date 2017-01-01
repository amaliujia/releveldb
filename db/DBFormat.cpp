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

bool ParseInternalKey(const Slice& internal_key,
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