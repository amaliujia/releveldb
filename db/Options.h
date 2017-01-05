//
// Created by rui_wang on 1/2/17.
//

#ifndef RELEVELDB_OPTIONS_H
#define RELEVELDB_OPTIONS_H

namespace releveldb {

class Snapshot;

// Options that control the read options.
struct ReadOptions {
  // if truy, all the read options from underlying storage will be
  // verified against checksums.
  // Default: false.
  bool verify_checksums;

  // If true, the read of iterations should be cached in memory
  // When do bulk scans this should be false (why?)
  // Default: True
  bool fill_cache;

  // If non-null, read operation can read from provided snapshot,
  // which must belong to the DB that is being read and must not be
  // released. If "snaphost" is null, use the implicit snapshot of the
  // state of the beginning of read option
  // Default: null
  const Snapshot* snapshot;

  ReadOptions(): verify_checksums(false),
                 fill_cache(true),
                 snapshot(nullptr) { }
};

}  // namespace releveldb

#endif //RELEVELDB_OPTIONS_H
