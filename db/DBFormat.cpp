//
// Created by 王 瑞 on 2016/11/27.
//


#include "DBFormat.h"

namespace releveldb {

static uint64_t PackSequenceAndType(uint64_t seq, ValueType t) {

}

void AppendInternalKey(std::string* result,
                       const ParsedInternalKey& key) {
  result->append(key.user_key.Data(), key.user_key.Size());
  PutFixed64(result, );

}

}  // namespace releveldb