//
// Created by amaliujia on 10/29/16.
//

#ifndef RELEVELDB_STATUS_H
#define RELEVELDB_STATUS_H

#include <string.h>

#include "db/Slice.h"

namespace releveldb {

class Status {
public:
  Status():status_(nullptr) {}

  ~Status() {
    delete[] status_;
  }

  std::string ToString() const;

private:
  const int codeOff = 4;

  enum StatusCode {
    KOK = 0,
    KNotFound = 1,
    KCorruption = 2,
    KNotSupproted = 3,
    KInvalidArgument = 4,
    KIOError = 5
  };

  StatusCode Code() const {
    return (status_ == nullptr) ? StatusCode::KOK : static_cast<StatusCode>(status_[codeOff]);
  }

  Status(StatusCode code, const Slice& msg, const Slice& msg2);

  // this a not a fixed length string.
  // stats_[0..3] the length of status message.
  // stats_[4] status code.
  // stats_[5...] the message.
  const char* status_;
};

}  // releveldb

#endif //RELEVELDB_STATUS_H
