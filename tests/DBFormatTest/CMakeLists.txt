include_directories("${PROJECT_SOURCE_DIR}/libs/googlemock/include"
        "${PROJECT_SOURCE_DIR}/libs/googletest/include"
        "${PROJECT_SOURCE_DIR}")

add_executable(dbformat_test DBFormatTests.cpp)
target_link_libraries(dbformat_test gtest gtest_main)
target_link_libraries(dbformat_test db)