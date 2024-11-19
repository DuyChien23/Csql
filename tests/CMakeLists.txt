file(GLOB_RECURSE Sources_test CONFIGURE_DEPENDS "*.cpp")

set(This_test Test)

add_executable(${This_test} ${Sources_test})

target_link_libraries(${This_test} PUBLIC
        gtest_main
        gmock_main
        source_libs
)

add_test(
        NAME ${This_test}
        COMMAND ${This_test}
)