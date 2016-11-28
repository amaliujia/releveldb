include_directories("${PROJECT_SOURCE_DIR}/libs/googlemock/include"
                    "${PROJECT_SOURCE_DIR}/libs/googletest/include"
                    "${PROJECT_SOURCE_DIR}")

add_executable(slice_test SliceTests.cpp)
target_link_libraries(slice_test gtest gtest_main)
target_link_libraries(slice_test db)