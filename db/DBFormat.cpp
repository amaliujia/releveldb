//
// Created by 王 瑞 on 2016/11/27.
//


#include "DBFormat.h"

namespace releveldb {

void AppendInternalKey(std::string* result,
                       const ParsedInternalKey& key) {
  result->append(key.user_key.Data(), key.user_key.Size());


}

}  // namespace releveldb