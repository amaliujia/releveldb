//
// Created by 王 瑞 on 2017/1/1.
//

#include <cstdlib>
#include "db/port_posix.h"

namespace releveldb {

void PthreadCall(const char* label, int result) {
  if (result != 0) {
    fprintf(stderr, "pthread %s : %s\n", label, strerror(result));
    abort();
  }
}

void InitOnce(OnceType* once, void (*initializer)()) {
  PthreadCall("Once", pthread_once(once, initializer));
}

}  // namespace releveldb