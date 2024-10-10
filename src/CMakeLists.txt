file(GLOB_RECURSE Sources CONFIGURE_DEPENDS "core/*.cpp" "core/*.h")

add_library(source_libs ${Sources})

add_executable(Csql main.cpp ${Sources})

target_link_libraries(Csql PUBLIC
        source_libs
        pthread
)

add_executable(Csql_test test.cpp ${Sources})

target_link_libraries(Csql_test PUBLIC
        source_libs
        pthread
)
