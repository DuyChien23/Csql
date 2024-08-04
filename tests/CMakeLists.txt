cmake_minimum_required(VERSION 3.28)

set(This CsqlTest)

set(Sources IO_test.cpp
        Entity_test.cpp
        LRUCache_test.cpp)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
        gtest_main
        source_libs
)

add_test(
    NAME ${This}
    COMMAND ${This}
)