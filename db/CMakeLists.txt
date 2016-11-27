include_directories("${PROJECT_SOURCE_DIR}")

add_library(db Slice.cpp
                Status.cpp
                Arena.cpp
                Iterator.cpp
                Env.cpp
                Env_posix.cpp
                random.h
                atomic_pointer.h
                MutexLock.h
                port_posix.h)