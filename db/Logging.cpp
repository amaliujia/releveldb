//
// Created by rui_wang on 12/31/16.
//

#include "Logging.h"


namespace releveldb {

void AppendEscapeStringto(std::string* target, const Slice& s) {
  for (size_t i = 0; i < s.Size(); i++) {
    char c = s[i];
    if (c >= ' ' && c <= '~') {
      target->push_back(c);
    } else {
      char buf[10];
      snprintf(buf, sizeof(buf), "\\x%02x", static_cast<unsigned int>(c) & 0xff);
      target->append(buf);
    }
  }
}

std::string EscapeString(const Slice& s) {
  std::string result;
  AppendEscapeStringto(&result, s);
  return result;
}

}  // namespace releveldb