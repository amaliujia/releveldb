include_directories("${PROJECT_SOURCE_DIR}/libs/googlemock/include"
        "${PROJECT_SOURCE_DIR}/libs/googletest/include"
        "${PROJECT_SOURCE_DIR}")

add_executable(arena_test ArenaTests.cpp)
target_link_libraries(arena_test gtest gtest_main)
target_link_libraries(arena_test db)