include_directories("${PROJECT_SOURCE_DIR}/libs/googlemock/include"
        "${PROJECT_SOURCE_DIR}/libs/googletest/include"
        "${PROJECT_SOURCE_DIR}")

add_executable(env_test EnvTests.cpp)
target_link_libraries(env_test gtest gtest_main)
target_link_libraries(env_test db)