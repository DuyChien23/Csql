//
// Created by chiendd on 27/07/2024.
//

#include <gtest/gtest.h>
#include "../src/core/storage/io_manager/io_manager.h"
#include "../src/core/util/errors.h"

using namespace Csql;

namespace Csql {
    class IoManagerTest : IOManager {
    public:
        IoManagerTest() = default;
        ~IoManagerTest() = default;

        void encode_decode_test(const std::string input) {
            std::fstream fs_write("io_test.txt", CREATING_DISK_MODE);
            strcpy(this->rawData, input.c_str());

            try {
                this->encode(fs_write);
            } catch (const Errors e) {
                FAIL() << e.what();
            }


            std::fstream fs_read("io_test.txt", MODIFYING_DISK_MODE);

            try {
                this->decode(fs_read);
            } catch (const Errors& e) {
                FAIL() << e.what();
            }

            EXPECT_EQ(this->rawData, input);
        };

        void write_read_int_test(uint64_t input, uint64_t buffer) {
            uint64_t output;

            this->bufferOffset = buffer;
            this->write(input);

            this->bufferOffset = buffer;
            this->read(output);

            EXPECT_EQ(input, output);
        }

        void write_read_string_test(std::string input, uint64_t buffer) {
            std::string output;

            this->bufferOffset = buffer;
            this->write(input);

            this->bufferOffset = buffer;
            this->read(output);

            EXPECT_EQ(input, output);
        }

        void write_read_data_types_test(SqlTypes input, DataTypes type, uint64_t buffer) {
            SqlTypes output;

            this->bufferOffset = buffer;
            this->write(input, type);

            this->bufferOffset = buffer;
            this->read(output, type);

            EXPECT_EQ(input, output);
        }
    } io_manager_test;
}

TEST(IO_TEST, ENCODE_DECODE_TEST) {
    io_manager_test.encode_decode_test("2ieoijmd1-dj10odkowd,op1wk,x-1wkx");
    io_manager_test.encode_decode_test("");
    io_manager_test.encode_decode_test("123D!ED!@E#FEGFG@$%34gef982jef2jd2iwr@F$@%$TGFEWF@EF@EF@E%Egdcfsdcgwetwet");
}


TEST(IO_TEST, WRITE_READ_STRING_TEST) {
    io_manager_test.write_read_string_test("123123124", 0);
    io_manager_test.write_read_string_test("123D!ED!@E#FEGFG@$%34gef982jef2jd2iwr@F$@%$TGFEWF@EF@EF@E%Egdcfsdcgwetwet", 13);
    io_manager_test.write_read_string_test("", 101);
}

TEST(IO_TEST, WRITE_READ_INT_TEST) {
    io_manager_test.write_read_int_test(1223, 0);
    io_manager_test.write_read_int_test(12223123, 100);
    io_manager_test.write_read_int_test(2317321312132321, 1000);
}

TEST(IO_TEST, WRITE_READ_DATA_TYPES_TEST) {
    io_manager_test.write_read_data_types_test(SqlIntType(1), DataTypes::int_type, 12);
    io_manager_test.write_read_data_types_test(SqlBoolType(true), DataTypes::bool_type, 23);
    io_manager_test.write_read_data_types_test(SqlFloatType(2.348), DataTypes::float_type, 156);
    io_manager_test.write_read_data_types_test(
        SqlVarcharType("3D!ED!@E#FEGFG@$%34gef982jef2jd2iwr@F$@%$TGFEWF@EF@EF@E"),
        DataTypes::varchar_type,
        279);
    io_manager_test.write_read_data_types_test(SqlNullType(),DataTypes::null_type,1000);
    io_manager_test.write_read_data_types_test(SqlDatetimeType(10),DataTypes::datetime_type,103);
}