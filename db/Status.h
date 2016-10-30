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

  Status(const Status& status) {
    status_ = (status.Code() == KOK) ? nullptr : CopyState(status.GetStatus());
  }

  void operator=(const Status& status) {
    status_ = (status.Code() == KOK) ? nullptr : CopyState(status.GetStatus());
  }

  ~Status() {
    if (status_ != nullptr) {
      delete[] status_;
    }
  }

  static Status OK() {
    return Status();
  }

  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(KNotFound, msg, msg2);
  }

  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(KCorruption, msg, msg2);
  }

  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(KNotSupproted, msg, msg2);
  }

  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(KInvalidArgument, msg, msg2);
  }

  static Status IOError(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(KIOError, msg, msg2);
  }

  // Returns true iff the status indicates success.
  bool ok() const { return (status_ == NULL); }

  // Returns true iff the status indicates a NotFound error.
  bool IsNotFound() const { return Code() == KNotFound; }

  // Returns true iff the status indicates a Corruption error.
  bool IsCorruption() const { return Code() == KCorruption; }

  // Returns true iff the status indicates an IOError.
  bool IsIOError() const { return Code() == KIOError; }

  // Returns true iff the status indicates a NotSupportedError.
  bool IsNotSupportedError() const { return Code() == KNotSupproted; }

  // Returns true iff the status indicates an InvalidArgument.
  bool IsInvalidArgument() const { return Code() == KInvalidArgument; }

  std::string ToString() const;

  const char* GetStatus() const {
    return status_;
  }

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

  const char* CopyState(const char* state) {
    uint32_t size;
    memcpy(&size, state, sizeof(size));
    char* buffer = new char[size + 5];
    memcpy(buffer, state, size + 5);
    return buffer;
  }

  // this a not a fixed length string.
  // stats_[0..3] the length of status message.
  // stats_[4] status code.
  // stats_[5...] the message.
  const char* status_;
};

}  // releveldb

#endif //RELEVELDB_STATUS_H
