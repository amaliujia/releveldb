//
// Created by 王 瑞 on 2017/6/18.
//

#ifndef RELEVELDB_LOGWRITTER_H
#define RELEVELDB_LOGWRITTER_H

#include "db/Log_format.h"
#include "db/Slice.h"
#include "db/Status.h"

namespace releveldb {

class WritableFile;

namespace log {

class Writer {
public:
  // Create a writer that will append data to "*dest".
  // "*dest" must be initially empty.
  // "*dest" must remain live while this Writer is in use.
  explicit Writer(WritableFile* dest);

  // Create a writer that will append data to "*dest".
  // "*dest" must have initial length "dest_length".
  // "*dest" must remain live while this Writer is in use.
  Writer(WritableFile* dest, uint64_t dest_length);

  ~Writer();
};

}
}  // namespace releveldb


#endif //RELEVELDB_LOGWRITTER_H
