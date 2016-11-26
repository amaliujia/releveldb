//
// Created by 王 瑞 on 2016/11/25.
//

#ifndef RELEVELDB_ENV_H
#define RELEVELDB_ENV_H

#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "db/Status.h"

namespace releveldb {

class Logger;
class FileLock;
class RandomAccessFile;
class Slice;
class WritableFile;
class SequentialFile;

class Env {
public:
  Env() {}
  virtual ~Env();

  static Env* Default();
};

class RandomAccessFile {
public:
  RandomAccessFile(const RandomAccessFile &file) = delete;

  void operator=(const RandomAccessFile &file) = delete;

  RandomAccessFile() {}

  virtual ~RandomAccessFile();

  virtual Status Read(uint64_t offset, size_t n, Slice *result, char *scratch) const = 0;
};

class WritableFile {
public:
  WritableFile(const WritableFile &) = delete;

  void operator=(const WritableFile &) = delete;

  WritableFile() {}

  virtual ~WritableFile();

  virtual Status Close() = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;
  virtual Status Append(const Slice& data) = 0;
};

class Logger {
public:
  Logger(const Logger &) = delete;

  void operator=(const Logger &) = delete;

  Logger() {}

  virtual ~Logger();

  // Write an entry to the log file with the specified format.
  virtual void Logv(const char *format, va_list ap) = 0;
};


class FileLock {
public:
  FileLock(const FileLock &) = delete;

  void operator=(const FileLock &) = delete;

  FileLock() {}

  virtual ~FileLock();
};

class SequentialFile {
public:
  // No copying allowed
  SequentialFile(const SequentialFile&) = delete;
  void operator=(const SequentialFile&) = delete;


  SequentialFile() { }
  virtual ~SequentialFile();

  // Read up to "n" bytes from the file.  "scratch[0..n-1]" may be
  // written by this routine.  Sets "*result" to the data that was
  // read (including if fewer than "n" bytes were successfully read).
  // May set "*result" to point at data in "scratch[0..n-1]", so
  // "scratch[0..n-1]" must be live when "*result" is used.
  // If an error was encountered, returns a non-OK status.
  //
  // REQUIRES: External synchronization
  virtual Status Read(size_t n, Slice* result, char* scratch) = 0;

  // Skip "n" bytes from the file. This is guaranteed to be no
  // slower that reading the same data, but may be faster.
  //
  // If end of file is reached, skipping will stop at the end of the
  // file, and Skip will return OK.
  //
  // REQUIRES: External synchronization
  virtual Status Skip(uint64_t n) = 0;
};

// Log the specified data to *info_log if info_log is non-NULL.
extern void Log(Logger* info_log, const char* format, ...)
#   if defined(__GNUC__) || defined(__clang__)
__attribute__((__format__ (__printf__, 2, 3)))
#   endif
;

// A utility routine: write "data" to the named file.
extern Status WriteStringToFile(Env* env, const Slice& data,
                                const std::string& fname);

// A utility routine: read contents of named file into *data
extern Status ReadFileToString(Env* env, const std::string& fname,
                               std::string* data);


}  // namespace releveldb

#endif //RELEVELDB_ENV_H
