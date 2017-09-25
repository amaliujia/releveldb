//
// Created by 王 瑞 on 2017/6/18.
//

#ifndef RELEVELDB_LOG_FORMAT_H
#define RELEVELDB_LOG_FORMAT_H

namespace releveldb {
namespace log {

enum RecordType {
  // Zero is reserved for preallocated files
  kZeroType = 0,

  kFullType = 1,

  // For fragments
  kFirstType = 2,
  kMiddleType = 3,
  kLastType = 4
};
static const int kMaxRecordType = kLastType;

// 32MB
static const int kBlockSize = 32768;

// Header is checksum (4 bytes), length (2 bytes), type (1 byte).
static const int kHeaderSize = 4 + 2 + 1;

}  // namespace log
}  // namespace level

#endif //RELEVELDB_LOG_FORMAT_H
