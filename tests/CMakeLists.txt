if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

add_subdirectory(SliceTest)

add_subdirectory(StatusTest)

add_subdirectory(ArenaTest)

add_subdirectory(EnvTests)