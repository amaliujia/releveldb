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

  virtual Status NewSequentialFile(const std::string& filename,
                                    SequentialFile** result) = 0;
  virtual Status NewRandomAccessFile(const std::string& filename,
                                      RandomAccessFile** result) = 0;
  virtual Status NewWritableFile(const std::string& filename,
                                  WritableFile** result) = 0;
  virtual Status NewAppendableFile(const std::string& filename,
                                  WritableFile** result) = 0;
  virtual bool FileExists(const std::string& filename) = 0;

  virtual Status GetChildren(const std::string& dir,
                            std::vector<std::string>* result) = 0;
  virtual Status DeleteFile(const std::string& filename) = 0;
  virtual Status CreateDir(const std::string& dir) = 0;
  virtual Status DeleteDir(const std::string& dir) = 0;
  virtual Status GetFileSize(const std::string& filename, uint64_t* file_size) = 0;
  virtual Status RenameFile(const std::string& src, const std::string& target) = 0;
  virtual Status LockFile(const std::string& filename, FileLock** locker) = 0;
  virtual Status UnlockFile(FileLock* lock) = 0;

  // Arrange to run "(*function)(arg)" once in a background thread.
  //
  // "function" may run in an unspecified thread.  Multiple functions
  // added to the same Env may run concurrently in different threads.
  // I.e., the caller may not assume that background work items are
  // serialized.
  virtual void Schedule(
  void (*function)(void* arg),
  void* arg
  ) = 0;

  virtual void StartThread(void (*function)(void* arg), void* arg) = 0;

  virtual Status GetTestDirectory(std::string* path) = 0;

  virtual Status NewLogger(const std::string& logName, Logger** result) = 0;

  virtual uint64_t NowMicros() = 0;

  virtual void SleepForMicroseconds(int micros) = 0;
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


class EnvWrapper : public Env {
public:
  // Initialize an EnvWrapper that delegates all calls to *t
  explicit EnvWrapper(Env* t) : target_(t) { }
  virtual ~EnvWrapper();

  // Return the target to which this Env forwards all calls
  Env* target() const { return target_; }

  // The following text is boilerplate that forwards all methods to target()
  Status NewSequentialFile(const std::string& f, SequentialFile** r) {
    return target_->NewSequentialFile(f, r);
  }
  Status NewRandomAccessFile(const std::string& f, RandomAccessFile** r) {
    return target_->NewRandomAccessFile(f, r);
  }
  Status NewWritableFile(const std::string& f, WritableFile** r) {
    return target_->NewWritableFile(f, r);
  }
  Status NewAppendableFile(const std::string& f, WritableFile** r) {
    return target_->NewAppendableFile(f, r);
  }
  bool FileExists(const std::string& f) { return target_->FileExists(f); }
  Status GetChildren(const std::string& dir, std::vector<std::string>* r) {
    return target_->GetChildren(dir, r);
  }
  Status DeleteFile(const std::string& f) { return target_->DeleteFile(f); }
  Status CreateDir(const std::string& d) { return target_->CreateDir(d); }
  Status DeleteDir(const std::string& d) { return target_->DeleteDir(d); }
  Status GetFileSize(const std::string& f, uint64_t* s) {
    return target_->GetFileSize(f, s);
  }
  Status RenameFile(const std::string& s, const std::string& t) {
    return target_->RenameFile(s, t);
  }
  Status LockFile(const std::string& f, FileLock** l) {
    return target_->LockFile(f, l);
  }
  Status UnlockFile(FileLock* l) { return target_->UnlockFile(l); }
  void Schedule(void (*f)(void*), void* a) {
    return target_->Schedule(f, a);
  }
  void StartThread(void (*f)(void*), void* a) {
    return target_->StartThread(f, a);
  }
  virtual Status GetTestDirectory(std::string* path) {
    return target_->GetTestDirectory(path);
  }
  virtual Status NewLogger(const std::string& fname, Logger** result) {
    return target_->NewLogger(fname, result);
  }
  uint64_t NowMicros() {
    return target_->NowMicros();
  }
  void SleepForMicroseconds(int micros) {
    target_->SleepForMicroseconds(micros);
  }
private:
  Env* target_;
};

}  // namespace releveldb

#endif //RELEVELDB_ENV_H
