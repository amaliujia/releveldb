include_directories("${PROJECT_SOURCE_DIR}")

add_library(db Slice.cpp
                Status.cpp
                Arena.cpp
                Iterator.cpp
                Env.cpp)