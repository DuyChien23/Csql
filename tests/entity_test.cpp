//
// Created by chiendd on 27/07/2024.
//

#include <gtest/gtest.h>

#include "../src/core/storage/entity.h"
#include "../src/core/storage/page/slotted_page.h"

using namespace Csql;

Entity initStandardEntity(
    uint64_t firstDataBlock,
    uint64_t firstFreeBlock,
    AttributeList &attributes)
{
    Entity entity_test("my_db", "my_table");
    entity_test.setFirstDataPage(firstDataBlock);
    entity_test.setFirstFreePage(firstFreeBlock);
    for (auto &attribute : attributes)
        entity_test.addAttribute(attribute);
    return entity_test;
}

TEST(ENTITY_TEST, SAVE_REFRESH_TEST) {
    uint64_t firstDataBlock = 7;
    uint64_t firstFreeBlock = 10;
    AttributeList attributes;

    attributes.push_back((new Attribute)->setName("id")->setType(DataTypes::int_type));
    attributes.push_back((new Attribute)->setName("name")->setType(DataTypes::varchar_type));
    attributes.push_back((new Attribute)->setName("date")->setType(DataTypes::datetime_type));
    attributes.push_back((new Attribute)->setName("isOk")->setType(DataTypes::bool_type));
    attributes.push_back((new Attribute)->setName("score")->setType(DataTypes::float_type));


    auto entity_test = initStandardEntity(firstDataBlock, firstFreeBlock, attributes);

    entity_test.save();
    entity_test.refresh();

    EXPECT_EQ(firstDataBlock, entity_test.getFirstDataPage());
    EXPECT_EQ(firstFreeBlock, entity_test.getFirstFreePage());

    for (auto &attribute : attributes) {
        Attribute* anAttribute = entity_test.getAttribute(attribute->getName());
        EXPECT_NE(anAttribute, nullptr);
        EXPECT_EQ(attribute->getType(), anAttribute->getType());
    }

    entity_test.refresh();
}

TEST(ENTITY_TEST, SAVE_REFRESH_ATTRIBUTE_TEST) {
    AttributeList attributes(0);
    auto entity_test = initStandardEntity(0, 0, attributes);
    Attribute* anAttribute = (new Attribute)->setName("name")->setType(DataTypes::datetime_type)->setNullable(true)->setPrimary(true)->setAutoIncrement(13);

    entity_test.resetBufferOffset();
    entity_test.writeAttribute(anAttribute);

    entity_test.resetBufferOffset();
    Attribute* attributeCopy = entity_test.readAttribute();

    EXPECT_EQ(anAttribute->getName(), attributeCopy->getName());
    EXPECT_EQ(anAttribute->getType(), attributeCopy->getType());
    EXPECT_EQ(anAttribute->getAutoIncrement(), attributeCopy->getAutoIncrement());
    EXPECT_EQ(anAttribute->isNull(), attributeCopy->isNull());
    EXPECT_EQ(anAttribute->isPrimary(), attributeCopy->isPrimary());
}

TEST(PAGE_TEST, SAVE_REFRESH_SLOTTED_PAGE) {
    uint64_t firstDataBlock = 7;
    uint64_t firstFreeBlock = 10;
    AttributeList attributes;

    attributes.push_back((new Attribute)->setName("id")->setType(DataTypes::int_type));
    attributes.push_back((new Attribute)->setName("name")->setType(DataTypes::varchar_type));
    attributes.push_back((new Attribute)->setName("date")->setType(DataTypes::datetime_type));
    attributes.push_back((new Attribute)->setName("isOk")->setType(DataTypes::bool_type));
    attributes.push_back((new Attribute)->setName("score")->setType(DataTypes::float_type));

    SharedEntityPtr entity_ptr = std::make_shared<Entity>(initStandardEntity(firstDataBlock, firstFreeBlock, attributes));

    uint32_t pageIndex = 0;
    uint32_t nextPage = 1;
    uint32_t numSlots = 3;

    SlottedPage slotted_page(pageIndex, entity_ptr);
    slotted_page.set_next_page(nextPage);

    std::vector<uint32_t> slots(numSlots);
    std::vector<Tuple> tuples(numSlots);

    tuples[0] = {
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("name", SqlVarcharType("chien")),
        std::make_pair("date", SqlDatetimeType(30)),
        std::make_pair("isOk", SqlBoolType(true)),
        std::make_pair("score", SqlFloatType(1.5)),
    };

    tuples[1] = {
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("name", SqlVarcharType("hanh")),
        std::make_pair("date", SqlDatetimeType(3213)),
        std::make_pair("isOk", SqlBoolType(false)),
        std::make_pair("score", SqlFloatType(113.5)),
    };

    tuples[2] = {
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("name", SqlVarcharType("chanh")),
        std::make_pair("date", SqlDatetimeType(123)),
        std::make_pair("isOk", SqlBoolType(true)),
        std::make_pair("score", SqlFloatType(57.5)),
    };

    slots[0] = Configs::storageUnitSize - slotted_page.getTupleSize(tuples[0]);
    slots[1] = slots[0] - slotted_page.getTupleSize(tuples[1]);
    slots[2] = slots[1] - slotted_page.getTupleSize(tuples[2]);

    slotted_page.addTuple(tuples[0]);
    slotted_page.addTuple(tuples[1]);
    slotted_page.addTuple(tuples[2]);

    slotted_page.save();
    slotted_page.refresh();

    EXPECT_EQ(slotted_page.get_next_page(), nextPage);
    EXPECT_EQ(slotted_page.get_num_slots(), numSlots);

    for (int i = 0; i < numSlots; ++i) {
        EXPECT_EQ(slotted_page.get_slots().at(i), slots.at(i));

        for (auto &attribute : *slotted_page.get_the_entity()->getAttributes()) {
            EXPECT_EQ(slotted_page.get_tuples().at(i).at(attribute->getName()),
                tuples.at(i).at(attribute->getName()));
        }
    }
}