file(GLOB_RECURSE Sources CONFIGURE_DEPENDS "core/*.cpp" "core/*.h")

add_library(source_libs ${Sources})

add_executable(Csql main.cpp ${Sources}
        core/storage/sql_query.cpp
        core/storage/sql_query.h
        core/storage/expression/where_expression.cpp
        core/storage/expression/where_expression.h
        core/storage/expression/expression.h
        core/storage/expression/unary_expression.h
        core/storage/expression/value_expression.h
        core/storage/expression/attribute_expression.h
        core/storage/expression/binary_expression.h
        core/storage/expression/arithmetic_expresstion.h
        core/storage/expression/other_expression.h
)

target_link_libraries(Csql PUBLIC
        source_libs
)


