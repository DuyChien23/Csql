//
// Created by chiendd on 22/07/2024.
//

#ifndef TYPES_H
#define TYPES_H

#include <variant>
#include <fstream>
#include <map>

#define MODIFYING_DISK_MODE std::fstream::in | std::fstream::out | std::fstream::binary
#define CREATING_DISK_MODE std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc

namespace Csql {
    enum class DataTypes {
        null_type = 'N',
        int_type = 'I',
        varchar_type = 'V',
        bool_type = 'B',
        datetime_type = 'D',
        float_type = 'F',
    };

    using SqlNullType = std::monostate;
    using SqlIntType = int64_t;
    using SqlVarcharType = std::string;
    using SqlBoolType = bool;
    using SqlDatetimeType = uint64_t;
    using SqlFloatType = double;

    using SqlTypes = std::variant<SqlNullType, SqlIntType, SqlVarcharType, SqlBoolType, SqlDatetimeType, SqlFloatType>;
    //
    // using Tuple = std::map<std::string, SqlTypes>;
    //
    // using JoinedTuple = std::map<std::pair<std::string, std::string>, SqlTypes>;
    // //TODO: create struct Tuple that can be used to represent data from both a single table and a joined table
}

#endif //TYPES_H
