//
// Created by amaliujia on 10/29/16.
//

#include "Status.h"

namespace releveldb {

Status::Status(StatusCode code, const Slice& msg, const Slice& msg2) {
  size_t msgLen = msg.Size();
  size_t msg2Len = msg2.Size();
  size_t statusLen = msgLen + (msg2Len ? (msg2Len + 2) : 0);
  char* result = new char[statusLen + 5];
  memcpy(result, &statusLen, sizeof(statusLen));
  result[codeOff] = static_cast<char>(code);
  memcpy(result + 5, msg.Data(), msgLen);
  if (msg2Len > 0) {
    result[msgLen + 5] = ':';
    result[msgLen + 6] = ' ';
    memcpy(result + msgLen + 7, msg2.Data(), msg2Len);
  }

  status_ = result;
}

std::string Status::ToString() const {
  if (status_ == nullptr) {
    return "OK";
  } else {

  }
}

}  // releveldb