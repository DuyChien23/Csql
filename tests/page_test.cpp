//
// Created by chiendd on 29/10/2024.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/core/storage/page/slotted_page.h"
#include "../src/core/storage/page/b_plus_node/internal_b_plus_node.h"
#include "../src/core/storage/page/b_plus_node/leaf_b_plus_node.h"


class LeafPageTest : public LeafBPlusNode, public ::testing::Test {
public:
    LeafPageTest() : LeafBPlusNode(0, std::make_shared<Entity>("db", "entity")) {
        auto theEntity = this->get_the_entity().get();
        theEntity->addAttribute((new Attribute)->setName("id")->setType(DataTypes::int_type));
        theEntity->addAttribute((new Attribute)->setName("name")->setType(DataTypes::varchar_type));
        theEntity->addAttribute((new Attribute)->setName("date")->setType(DataTypes::datetime_type));
    };
};


TEST_F(LeafPageTest, SaveRefreshTuple) {
    Tuple tuple0 = {
        std::make_pair(SpecialKey::BTREE_KEY, BPlusKey(1)),
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("name", SqlVarcharType("chien1")),
        std::make_pair("date", SqlDatetimeType(30)),
    };

    Tuple tuple1 = {
        std::make_pair(SpecialKey::BTREE_KEY, BPlusKey(2)),
        std::make_pair("id", SqlIntType(2)),
        std::make_pair("name", SqlVarcharType("chien2")),
        std::make_pair("date", SqlDatetimeType(30)),
    };

    Tuple tuple2 = {
        std::make_pair(SpecialKey::BTREE_KEY, BPlusKey(3)),
        std::make_pair("id", SqlIntType(2)),
        std::make_pair("name", SqlVarcharType("chien2")),
        std::make_pair("date", SqlDatetimeType(30)),
    };

    addTuple(tuple2);
    addTuple(tuple0);
    addTuple(tuple1);

    saveType();
    save();

    loadType();
    refresh();

    auto compareTuple = [&](Tuple& a, Tuple& b) {
        ASSERT_EQ(a.at(SpecialKey::BTREE_KEY), b.at(SpecialKey::BTREE_KEY));
        ASSERT_EQ(a.at("id"), b.at("id"));
        ASSERT_EQ(a.at("name"), b.at("name"));
        ASSERT_EQ(a.at("date"), b.at("date"));
    };

    compareTuple(tuples[0], tuple0);
    compareTuple(tuples[1], tuple1);
    compareTuple(tuples[2], tuple2);
}

class InternalPageTest : public InternalBPlusNode, public ::testing::Test {
public:
    InternalPageTest() : InternalBPlusNode(0, std::make_shared<Entity>("db", "entity")) {
        auto theEntity = this->get_the_entity().get();
        theEntity->addAttribute((new Attribute)->setName("id")->setType(DataTypes::int_type));
        theEntity->addAttribute((new Attribute)->setName("name")->setType(DataTypes::varchar_type));
        theEntity->addAttribute((new Attribute)->setName("date")->setType(DataTypes::datetime_type));
    };
};

TEST_F(InternalPageTest, SaveRefreshTuple) {
    Tuple tuple0 = {
        std::make_pair(SpecialKey::BTREE_KEY, BPlusKey(1)),
        std::make_pair(SpecialKey::CHILD_BTREE_KEY, SqlIntType(1)),
    };

    Tuple tuple1 = {
        std::make_pair(SpecialKey::BTREE_KEY, BPlusKey(2)),
        std::make_pair(SpecialKey::CHILD_BTREE_KEY, SqlIntType(3)),
    };

    Tuple tuple2 = {
        std::make_pair(SpecialKey::BTREE_KEY, BPlusKey(3)),
        std::make_pair(SpecialKey::CHILD_BTREE_KEY, SqlIntType(5)),
    };

    addTuple(tuple0);
    addTuple(tuple2);
    addTuple(tuple1);

    saveType();
    save();

    loadType();
    refresh();

    auto compareTuple = [&](Tuple& a, Tuple& b) {
        ASSERT_EQ(a.at(SpecialKey::BTREE_KEY), b.at(SpecialKey::BTREE_KEY));
        ASSERT_EQ(a.at(SpecialKey::CHILD_BTREE_KEY), b.at(SpecialKey::CHILD_BTREE_KEY));
    };

    compareTuple(tuples[0], tuple0);
    compareTuple(tuples[1], tuple1);
    compareTuple(tuples[2], tuple2);
}