cmake_minimum_required(VERSION 3.28)

set(This CsqlTest)

set(Sources io_test.cpp
        entity_test.cpp
        lru_cache_test.cpp
        strorage_test.cpp
        main.cpp
        expression/arithmetic_expression_test.cpp
        expression/expression_test.h
        expression/attribute_expression_test.cpp
        expression/binary_expression_test.cpp
        expression/other_expression.cpp)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
        gtest_main
        gmock_main
        source_libs
)

add_test(
    NAME ${This}
    COMMAND ${This}
)