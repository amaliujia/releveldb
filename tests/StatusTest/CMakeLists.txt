include_directories("${PROJECT_SOURCE_DIR}/libs/googlemock/include"
                    "${PROJECT_SOURCE_DIR}/libs/googletest/include"
                    "${PROJECT_SOURCE_DIR}")

add_executable(status_test StatusTests.cpp)
target_link_libraries(status_test gtest gtest_main)
target_link_libraries(status_test db)
#target_link_libraries (cuckoo_hasing_table_basic_test glog)