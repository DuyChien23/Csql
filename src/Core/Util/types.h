//
// Created by chiendd on 22/07/2024.
//

#ifndef TYPES_H
#define TYPES_H
#include <ios>
#include <variant>
#include <fstream>
#include <map>


namespace Csql {
    enum class DataTypes {
        null_type = 'N',
        int_type = 'I',
        varchar_type = 'V',
        bool_type = 'B',
        datetime_type = 'D',
        float_type = 'F',
    };


    enum class DiskMode {
        read = std::fstream::in | std::fstream::out | std::fstream::binary,
        write = std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc,
    };

    using SqlNullType = std::monostate;
    using SqlIntType = int64_t;
    using SqlVarcharType = std::string;
    using SqlBoolType = bool;
    using SqlDatetimeType = uint64_t;
    using SqlFloatType = double;

    using SqlTypes = std::variant<SqlNullType, SqlIntType, SqlVarcharType, SqlBoolType, SqlDatetimeType, SqlFloatType>;

    using Tuple = std::map<std::string, SqlTypes>;
}

#endif //TYPES_H
