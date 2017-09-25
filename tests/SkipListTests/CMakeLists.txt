include_directories("${PROJECT_SOURCE_DIR}/libs/googlemock/include"
        "${PROJECT_SOURCE_DIR}/libs/googletest/include"
        "${PROJECT_SOURCE_DIR}")

add_executable(skiplist_test SkipListTests.cpp)
target_link_libraries(skiplist_test gtest gtest_main)
target_link_libraries(skiplist_test db)