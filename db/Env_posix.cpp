//
// Created by 王 瑞 on 2016/11/26.
//

#include "db/Env.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <deque>
#include <set>

#include "db/port_posix.h"
#include "db/MutexLock.h"


namespace releveldb{

namespace {

static Status IOError(const std::string& context,  int err_number) {
  return Status::IOError(context, strerror(err_number));
}

class PosixSequentialFile : public SequentialFile {
public:
  PosixSequentialFile(const std::string& name, FILE* f)
    : filename_(name), file_(f) { }

  virtual ~PosixSequentialFile() {
    fclose(file_);
  }

  virtual Status Read(size_t n, Slice* result, char* scratch) {
    Status s;
    // read n 1 byte from file_ and save to scratch.
    size_t byte_read = fread(scratch, 1, n, file_);

    *result = Slice(scratch, byte_read);
    if (byte_read < n) {
      if (feof(file_)) {
        // if hit the end of file, return ok
        s = Status::OK();
      } else {
        // This is a partial read with an error, return non-ok status.
        s = IOError(filename_, errno);
      }
    }
    return s;
  }

  virtual Status Skip(uint64_t n) {
    if (fseek(file_, n, SEEK_CUR)) {
      return IOError(filename_, errno);
    } else {
      return Status::OK();
    }
  }

private:
  std::string filename_;
  FILE* file_;
};

class PosixRandomAccessFile : public RandomAccessFile {
public:
  PosixRandomAccessFile(const std::string& name, int fd)
    :filename_(name), fd_(fd) { }
  virtual ~PosixRandomAccessFile() { close(fd_); }

  virtual Status Read(uint64_t offset, size_t n, Slice* result,
                      char* scratch) const {
    Status s = Status::OK();
    ssize_t byte_read = pread(fd_, scratch, n, static_cast<off_t>(offset));
    *result = Slice(scratch, (byte_read < 0) ? 0 : byte_read);
    if (byte_read < 0) {
      s = IOError(filename_, errno);
    }
    return s;
  }

private:
  std::string filename_;
  int fd_;
};

}  // namespace

}  // namespace releveldb