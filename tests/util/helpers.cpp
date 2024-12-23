#include <gtest/gtest.h>
#include "../../src/core/util/helpers.h"

#include <gmock/gmock-matchers.h>

#include "../../src/core/util/types.h"

using namespace Helpers;

TEST(SqlTypeHandleTest, sizeOfSqlType) {
    BPlusKey key;
    key.add(1);
    key.add("12");
    EXPECT_EQ(SqlTypeHandle::sizeOfSqlType(key), 30);
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
    EXPECT_EQ(SqlTypeHandle::sizeOfTuple(tuple), 4 + 2 + 8 + 4 + 4 + 4 + 5 + 4 + 4 + 8 + 4 + 3 + 30);
}

TEST(TupleHandleTest, genBNodeKey) {
    Tuple tuple = {
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("name", SqlVarcharType("chien")),
        std::make_pair("date", SqlDatetimeType(30)),
    };

    IndexingMetadata indexingMetadata("table", {"id", "name"}, true);

    auto key = TupleHandle::genBNodeKey(tuple, indexingMetadata);

    BPlusKey expectedKey;
    expectedKey.add(1);
    expectedKey.add("chien");

    EXPECT_EQ(key, expectedKey);
}