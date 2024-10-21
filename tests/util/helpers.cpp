#include <gtest/gtest.h>
#include "../../src/core/util/helpers.h"
#include "../../src/core/util/types.h"

using namespace Csql;
using namespace Helpers;

TEST(SqlTypeHandleTest, sizeOfSqlType) {
    BPlusKey key;
    key.add(1);
    key.add("12");
    EXPECT_EQ(SqlTypeHandle::sizeOfSqlType(key), 26);
}

TEST(SqlTypeHandleTest, sizeOfTuple) {
    BPlusKey key;
    key.add(1);
    key.add("12");

    Tuple tuple = {
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("name", SqlVarcharType("chien")),
        std::make_pair("date", SqlDatetimeType(30)),
        std::make_pair("xyz", key),
    };
    EXPECT_EQ(SqlTypeHandle::sizeOfTuple(tuple), 4 + 4 * 5 + 18 + 8 + 8 + 26);
}