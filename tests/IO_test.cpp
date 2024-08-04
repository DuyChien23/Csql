//
// Created by chiendd on 27/07/2024.
//

#include <gtest/gtest.h>
#include "../src/Core/Storage/IO_Manager/io_manager.h"

namespace Csql {
    class IoManagerTest : IOManager {
    public:
        IoManagerTest() = default;
        ~IoManagerTest() = default;

        void encode_decode_test(const std::string input) {
            std::fstream fs_write("io_test.txt", static_cast<std::ios_base::openmode>(DiskMode::write));
            strcpy(this -> rawData, input.c_str());
            this -> encode(fs_write);

            std::fstream fs_read("io_test.txt", static_cast<std::ios_base::openmode>(DiskMode::read));
            this -> decode(fs_read);
            EXPECT_EQ(this -> rawData, input);
        };

        void write_read_int_test(uint64_t input, uint64_t buffer) {
            uint64_t output;

            this -> bufferOffset = buffer;
            this -> write(input);

            this -> bufferOffset = buffer;
            this -> read(output);

            EXPECT_EQ(input, output);
        }

        void write_read_string_test(std::string input, uint64_t buffer) {
            std::string output;

            this -> bufferOffset = buffer;
            this -> write(input);

            this -> bufferOffset = buffer;
            this -> read(output);

            EXPECT_EQ(input, output);
        }

        void write_read_data_types_test(SqlTypes input, DataTypes type, uint64_t buffer) {
            SqlTypes output;

            this -> bufferOffset = buffer;
            this -> write(input, type);

            this -> bufferOffset = buffer;
            this -> read(output, type);

            EXPECT_EQ(input, output);
        }
    } io_manager_test;
}

TEST(IO_TEST, ENCODE_DECODE_TEST) {
    Csql::io_manager_test.encode_decode_test("2ieoijmd1-dj10odkowd,op1wk,x-1wkx");
    Csql::io_manager_test.encode_decode_test("");
    Csql::io_manager_test.encode_decode_test("123D!ED!@E#FEGFG@$%34gef982jef2jd2iwr@F$@%$TGFEWF@EF@EF@E%Egdcfsdcgwetwet");
}


TEST(IO_TEST, WRITE_READ_STRING_TEST) {
    Csql::io_manager_test.write_read_string_test("123123124", 0);
    Csql::io_manager_test.write_read_string_test("123D!ED!@E#FEGFG@$%34gef982jef2jd2iwr@F$@%$TGFEWF@EF@EF@E%Egdcfsdcgwetwet", 13);
    Csql::io_manager_test.write_read_string_test("", 101);
}

TEST(IO_TEST, WRITE_READ_INT_TEST) {
    Csql::io_manager_test.write_read_int_test(1223, 0);
    Csql::io_manager_test.write_read_int_test(12223123, 100);
    Csql::io_manager_test.write_read_int_test(2317321312132321, 1000);
}

TEST(IO_TEST, WRITE_READ_DATA_TYPES_TEST) {
    Csql::io_manager_test.write_read_data_types_test(Csql::SqlIntType(1), Csql::DataTypes::int_type, 12);
    Csql::io_manager_test.write_read_data_types_test(Csql::SqlBoolType(true), Csql::DataTypes::bool_type, 23);
    Csql::io_manager_test.write_read_data_types_test(Csql::SqlFloatType(2.348), Csql::DataTypes::float_type, 156);
    Csql::io_manager_test.write_read_data_types_test(
        Csql::SqlVarcharType("3D!ED!@E#FEGFG@$%34gef982jef2jd2iwr@F$@%$TGFEWF@EF@EF@E"),
        Csql::DataTypes::varchar_type,
        279);
    Csql::io_manager_test.write_read_data_types_test(Csql::SqlNullType(),Csql::DataTypes::null_type,1000);
    Csql::io_manager_test.write_read_data_types_test(Csql::SqlDatetimeType(10),Csql::DataTypes::datetime_type,103);
}