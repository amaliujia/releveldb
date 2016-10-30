//
// Created by amaliujia on 10/29/16.
//

#include <iostream>
#include "Status.h"

namespace releveldb {

Status::Status(StatusCode code, const Slice& msg, const Slice& msg2) {
  uint32_t msgLen = msg.Size();
  uint32_t msg2Len = msg2.Size();
  uint32_t statusLen = msgLen + (msg2Len ? (msg2Len + 2) : 0);
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
    char tmp[30];
    const char* type;
    switch (Code()) {
      case StatusCode::KNotFound:
        type = "Not Found: ";
        break;
      case StatusCode::KInvalidArgument:
        type = "Invalid Argument: ";
        break;
      case StatusCode::KCorruption:
        type = "Corruption: ";
        break;
      case StatusCode::KNotSupproted:
        type = "Not Supported: ";
        break;
      case StatusCode::KIOError:
        type = "IO Error: ";
        break;
      default:
        snprintf(tmp, sizeof(tmp), "Unknown code(%d): ",
                 static_cast<int>(Code()));
        type = tmp;
        break;
    }
    std::string result(type);
    uint32_t msgLength;
    memcpy(&msgLength, status_, sizeof(msgLength));
    result.append(status_ + 5, msgLength);
    return result;
  }
}

}  // releveldb