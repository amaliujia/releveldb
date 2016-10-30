include_directories("${PROJECT_SOURCE_DIR}/libs/googlemock/include"
                    "${PROJECT_SOURCE_DIR}/libs/googletest/include"
                    "${PROJECT_SOURCE_DIR}/CuckoohashingTable")

add_executable(cuckoo_hasing_table_basic_test
                basic.cpp)
target_link_libraries(cuckoo_hasing_table_basic_test gtest gtest_main)
#target_link_libraries (cuckoo_hasing_table_basic_test glog)

if (APPLE)
add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
add_definitions(-D__GLIBCXX__)
endif (APPLE)