//
// Created by 王 瑞 on 2016/11/29.
//

#ifndef RELEVELDB_CODING_H
#define RELEVELDB_CODING_H

#include "db/Common.h"
#include "db/port_posix.h"

namespace releveldb {

extern void PutFixed64(std::string* dst, uint64_t value);

extern void EncodeFixed64(char* dst, uint64_t value);

inline uint32_t DecodeFixed32(const char* ptr) {
 if (kLittleEndian) {
   uint32_t result;
   memcpy(&result, ptr, sizeof(result));
   return result;
 } else {
   return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0])))
           | (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8)
           | (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16)
           | (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
 }
}

inline uint64_t DecodeFixed64(const char* ptr) {
  if (kLittleEndian) {
    // Load the raw bytes
    uint64_t result;
    memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
    return result;
  } else {
    uint64_t lo = DecodeFixed32(ptr);
    uint64_t hi = DecodeFixed32(ptr + 4);
    return (hi << 32) | lo;
  }
}

// Pointer-based variants of GetVarint...  These either store a value
// in *v and return a pointer just past the parsed value, or return
// NULL on error.  These routines only look at bytes in the range
// [p..limit-1]
extern const char* GetVarint32Ptr(const char* p,const char* limit, uint32_t* v);
extern const char* GetVarint64Ptr(const char* p,const char* limit, uint64_t* v);

// Internal routine for use by fallback path of GetVarint32Ptr
extern const char* GetVarint32PtrFallback(const char* p,
                                          const char* limit,
                                          uint32_t* value);
inline const char* GetVarint32Ptr(const char* p,
                                  const char* limit,
                                  uint32_t* value) {
  if (p < limit) {
    uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
    if ((result & 128) == 0) {
      *value = result;
      return p + 1;
    }
  }
  return GetVarint32PtrFallback(p, limit, value);
}

}  // namespace releveldb

#endif //RELEVELDB_CODING_H
