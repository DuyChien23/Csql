//
// Created by chiendd on 27/07/2024.
//

#include <gtest/gtest.h>

#include "../src/Core/Storage/entity.h"
#include "../src/Core/Storage/Page/slotted_page.h"

Csql::Entity initStandardEntity(
    uint64_t firstDataBlock,
    uint64_t firstFreeBlock,
    Csql::AttributeList &attributes)
{
    Csql::Entity entity_test("my_db", "my_table");
    entity_test.setFirstDataBlock(firstDataBlock);
    entity_test.setFirstFreeBlock(firstFreeBlock);
    for (auto &attribute : attributes)
        entity_test.addAttribute(attribute);
    return entity_test;
}

TEST(ENTITY_TEST, SAVE_REFRESH_TEST) {
    uint64_t firstDataBlock = 7;
    uint64_t firstFreeBlock = 10;
    Csql::AttributeList attributes;

    attributes.push_back((new Csql::Attribute)->setName("id")->setType(Csql::DataTypes::int_type));
    attributes.push_back((new Csql::Attribute)->setName("name")->setType(Csql::DataTypes::varchar_type));
    attributes.push_back((new Csql::Attribute)->setName("date")->setType(Csql::DataTypes::datetime_type));
    attributes.push_back((new Csql::Attribute)->setName("isOk")->setType(Csql::DataTypes::bool_type));
    attributes.push_back((new Csql::Attribute)->setName("score")->setType(Csql::DataTypes::float_type));


    auto entity_test = initStandardEntity(firstDataBlock, firstFreeBlock, attributes);

    entity_test.save();
    entity_test.refresh();

    EXPECT_EQ(firstDataBlock, entity_test.getFirstDataBlock());
    EXPECT_EQ(firstFreeBlock, entity_test.getFirstFreeBlock());

    for (auto &attribute : attributes) {
        Csql::Attribute* anAttribute = entity_test.getAttribute(attribute -> getName());
        EXPECT_NE(anAttribute, nullptr);
        EXPECT_EQ(attribute -> getType(), anAttribute -> getType());
    }

    entity_test.refresh();
}

TEST(ENTITY_TEST, SAVE_REFRESH_ATTRIBUTE_TEST) {
    Csql::AttributeList attributes(0);
    auto entity_test = initStandardEntity(0, 0, attributes);
    Csql::Attribute* anAttribute = (new Csql::Attribute)->setName("name")->setType(Csql::DataTypes::datetime_type)->setNullable(true)->setPrimary(true)->setAutoIncrement(13);

    entity_test.resetBufferOffset();
    entity_test.writeAttribute(anAttribute);

    entity_test.resetBufferOffset();
    Csql::Attribute* attributeCopy = entity_test.readAttribute();

    EXPECT_EQ(anAttribute -> getName(), attributeCopy -> getName());
    EXPECT_EQ(anAttribute -> getType(), attributeCopy -> getType());
    EXPECT_EQ(anAttribute -> getAutoIncrement(), attributeCopy -> getAutoIncrement());
    EXPECT_EQ(anAttribute -> isNull(), attributeCopy -> isNull());
    EXPECT_EQ(anAttribute -> isPrimary(), attributeCopy -> isPrimary());
}

TEST(PAGE_TEST, SAVE_REFRESH_SLOTTED_PAGE) {
    uint64_t firstDataBlock = 7;
    uint64_t firstFreeBlock = 10;
    Csql::AttributeList attributes;

    attributes.push_back((new Csql::Attribute)->setName("id")->setType(Csql::DataTypes::int_type));
    attributes.push_back((new Csql::Attribute)->setName("name")->setType(Csql::DataTypes::varchar_type));
    attributes.push_back((new Csql::Attribute)->setName("date")->setType(Csql::DataTypes::datetime_type));
    attributes.push_back((new Csql::Attribute)->setName("isOk")->setType(Csql::DataTypes::bool_type));
    attributes.push_back((new Csql::Attribute)->setName("score")->setType(Csql::DataTypes::float_type));

    Csql::SharedEntityPtr entity_ptr = std::make_shared<Csql::Entity>(initStandardEntity(firstDataBlock, firstFreeBlock, attributes));

    uint32_t pageIndex = 0;
    uint32_t nextPage = 1;
    uint32_t beginFreeSpace = 10;
    uint32_t endFreeSpace = 100;
    uint32_t numSlots = 3;

    Csql::SlottedPage slotted_page(pageIndex, entity_ptr);
    slotted_page.set_next_page(nextPage);
    slotted_page.set_begin_free_space(beginFreeSpace);
    slotted_page.set_end_free_space(endFreeSpace);
    slotted_page.set_num_slots(numSlots);

    std::vector<uint32_t> slots(numSlots);
    std::vector<Csql::Tuple> tuples(numSlots);

    tuples[0] = {
        std::make_pair("id", Csql::SqlIntType(1)),
        std::make_pair("name", Csql::SqlVarcharType("chien")),
        std::make_pair("date", Csql::SqlDatetimeType(30)),
        std::make_pair("isOk", Csql::SqlBoolType(true)),
        std::make_pair("score", Csql::SqlFloatType(1.5)),
    };

    tuples[1] = {
        std::make_pair("id", Csql::SqlIntType(1)),
        std::make_pair("name", Csql::SqlVarcharType("hanh")),
        std::make_pair("date", Csql::SqlDatetimeType(3213)),
        std::make_pair("isOk", Csql::SqlBoolType(false)),
        std::make_pair("score", Csql::SqlFloatType(113.5)),
    };

    tuples[2] = {
        std::make_pair("id", Csql::SqlIntType(1)),
        std::make_pair("name", Csql::SqlVarcharType("chanh")),
        std::make_pair("date", Csql::SqlDatetimeType(123)),
        std::make_pair("isOk", Csql::SqlBoolType(true)),
        std::make_pair("score", Csql::SqlFloatType(57.5)),
    };

    slots[2] = Config::storageUnitSize - slotted_page.getTupleSize(tuples[2]);
    slots[1] = slots[2] - slotted_page.getTupleSize(tuples[1]);
    slots[0] = slots[1] - slotted_page.getTupleSize(tuples[0]);

    slotted_page.set_slots(slots);
    slotted_page.set_tuples(tuples);

    slotted_page.save();
    slotted_page.refresh();

    EXPECT_EQ(slotted_page.get_next_page(), nextPage);
    EXPECT_EQ(slotted_page.get_begin_free_space(), beginFreeSpace);
    EXPECT_EQ(slotted_page.get_end_free_space(), endFreeSpace);
    EXPECT_EQ(slotted_page.getFreeSpace(), endFreeSpace - beginFreeSpace);
    EXPECT_EQ(slotted_page.get_num_slots(), numSlots);

    for (int i = 0; i < numSlots; ++i) {
        EXPECT_EQ(slotted_page.get_slots().at(i), slots.at(i));

        for (auto &attribute : *slotted_page.get_the_entity()->getAttributes()) {
            EXPECT_EQ(slotted_page.get_tuples().at(i).at(attribute->getName()),
                tuples.at(i).at(attribute->getName()));
        }
    }
}