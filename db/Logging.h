//
// Created by rui_wang on 12/31/16.
//

#ifndef RELEVELDB_LOGGING_H
#define RELEVELDB_LOGGING_H

#include <string>
#include "db/Slice.h"

namespace releveldb {

extern void AppendEscapeStringto(std::string* target, const Slice& s);

// return a human-readable string, escpae any
// non-printable characters.
extern std::string EscapeString(const Slice& s);

}  // namespace releveldb

#endif //RELEVELDB_LOGGING_H
