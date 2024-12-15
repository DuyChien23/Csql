//
// Created by chiendd on 24/10/2024.
//

#include "helpers.h"

#include "errors.h"
using namespace Helpers;

//==========================================SQL-TYPE-HANDLE===================================================
std::string SqlTypeHandle::covertSqlTypeToString(const SqlTypes &value) {
    switch (value.index()) {
        case sql_types_index_v<SqlNullType>:
            return "NULL";
        case sql_types_index_v<SqlBoolType>:
            return std::get<SqlBoolType>(value) ? "True" : "False";
        case sql_types_index_v<SqlDatetimeType>:
            return std::to_string(std::get<SqlDatetimeType>(value));
        case sql_types_index_v<SqlIntType>:
            return std::to_string(std::get<SqlIntType>(value));
        case sql_types_index_v<SqlFloatType>:
            return std::to_string(std::get<SqlFloatType>(value));
        case sql_types_index_v<SqlVarcharType>:
            return std::get<SqlVarcharType>(value);
        default:
            return "";
    }
}

DataTypes SqlTypeHandle::covertSqlValueToDataTypes(const SqlTypes &value) {
    switch (value.index()) {
        case sql_types_index_v<SqlNullType>:
            return DataTypes::null_type;
        case sql_types_index_v<SqlBoolType>:
            return DataTypes::bool_type;
        case sql_types_index_v<SqlDatetimeType>:
            return DataTypes::datetime_type;
        case sql_types_index_v<SqlIntType>:
            return DataTypes::int_type;
        case sql_types_index_v<SqlFloatType>:
            return DataTypes::float_type;
        case sql_types_index_v<SqlVarcharType>:
            return DataTypes::varchar_type;
        default:
            throw Errors("Can't convert unknown type to data type");
    }
}

DataTypes SqlTypeHandle::covertStringToDataType(std::string type) {
    if (type == "bool") return DataTypes::bool_type;
    if (type == "date") return DataTypes::datetime_type;
    if (type == "int") return DataTypes::int_type;
    if (type == "integer") return DataTypes::int_type;
    if (type == "float") return DataTypes::float_type;
    if (type == "varchar") return DataTypes::varchar_type;
    return DataTypes::null_type;
}

SqlTypes SqlTypeHandle::covertStringToSqlType(DataTypes type, std::string value) {
    if (type == DataTypes::bool_type) {
        for (auto &c: value)
            c = std::tolower(c);

        if (value == "true") return SqlBoolType(true);
        if (value == "false") return SqlBoolType(false);
        throw Errors("Invalid value for bool type");
    }
    if (type == DataTypes::int_type) {
        try {
            return SqlIntType(std::stoi(value));
        } catch (...) {
            throw Errors("Invalid value for int type");
        }
    }
    if (type == DataTypes::float_type) {
        try {
            return SqlFloatType(std::stof(value));
        } catch (...) {
            throw Errors("Invalid value for float type");
        }
    }
    if (type == DataTypes::datetime_type) {
        try {
            return SqlDatetimeType(std::stoull(value));
        } catch (...) {
            throw Errors("Invalid value for datetime type");
        }
    }
    if (type == DataTypes::varchar_type) {
        return SqlVarcharType(value);
    }
    return SqlNullType();
}

uint32_t SqlTypeHandle::sizeOfSqlType(const SqlTypes &value) {
    switch (value.index()) {
        case sql_types_index_v<SqlBoolType>:
            return sizeof(SqlBoolType);
        case sql_types_index_v<SqlDatetimeType>:
            return sizeof(SqlDatetimeType);
        case sql_types_index_v<SqlIntType>:
            return sizeof(SqlIntType);
        case sql_types_index_v<SqlFloatType>:
            return sizeof(SqlFloatType);
        case sql_types_index_v<SqlVarcharType>:
            return sizeof(uint32_t) + std::get<SqlVarcharType>(value).size();
        case sql_types_index_v<BPlusKey>: {
            BPlusKey temp = std::get<BPlusKey>(value);
            return temp.getRawSize();
        }
        default:
            return 0;
    }
}

uint32_t SqlTypeHandle::sizeOfTuple(const Tuple &aTuple) {
    uint32_t result = 0;
    for (auto &element: aTuple) {
        result += sizeof(uint32_t);
        result += element.first.size();
        result += sizeOfSqlType(element.second);
    }
    return result;
}

std::string SqlTypeHandle::covertDataTypeToString(const DataTypes &type) {
    switch (type) {
        case DataTypes::bool_type:
            return "bool";
        case DataTypes::datetime_type:
            return "date";
        case DataTypes::float_type:
            return "float";
        case DataTypes::int_type:
            return "integer";
        case DataTypes::null_type:
            return "null";
        case DataTypes::varchar_type:
            return "varchar";
        default:
            return "";
    }
}

PairKeyElement SqlTypeHandle::convertToPairKeyElement(const SqlTypes &value) {
    KeyElement key;

    switch (value.index()) {
        case sql_types_index_v<SqlNullType>:
            throw Errors("Can't convert null type to pair key element");
        case sql_types_index_v<SqlBoolType>:
            throw Errors("Can't convert bool type to pair key element");
        case sql_types_index_v<SqlDatetimeType>:
            key = int64_t(std::get<SqlDatetimeType>(value));
            return std::make_pair(int_type, key);
        case sql_types_index_v<SqlIntType>:
            key = int64_t(std::get<SqlIntType>(value));
            return std::make_pair(int_type, key);
        case sql_types_index_v<SqlFloatType>:
            key = double(std::get<SqlFloatType>(value));
            return std::make_pair(float_type, key);
        case sql_types_index_v<SqlVarcharType>:
            return std::make_pair(varchar_type, std::get<SqlVarcharType>(value));
        default:
            throw Errors("Can't convert unknown type to pair key element");
    }
}

//==========================================EXPRESSION-HANDLE===================================================
SqlTypeCompareResult ExpressionHandle::compareSqlType(const SqlTypes &lhs, const SqlTypes &rhs) {
    if (lhs.index() != rhs.index()) return SqlTypeCompareResult::cant_compare;

    if (std::holds_alternative<SqlBoolType>(lhs) || std::holds_alternative<SqlNullType>(lhs)) {
        return SqlTypeCompareResult::cant_compare;
    }

    if (std::holds_alternative<SqlVarcharType>(lhs)) {
        return lhs == rhs ? SqlTypeCompareResult::equal : SqlTypeCompareResult::not_equal;
    }

    if (lhs == rhs) return SqlTypeCompareResult::equal;

    return lhs < rhs ? SqlTypeCompareResult::less_than : SqlTypeCompareResult::greater;
}

bool ExpressionHandle::likeMatch(const SqlTypes &lhs, const SqlTypes &rhs) {
    if (!std::holds_alternative<SqlVarcharType>(lhs) || !std::holds_alternative<SqlVarcharType>(rhs)) {
        throw Errors("Type of operand invalid");
    }

    std::string str = std::get<SqlVarcharType>(lhs);
    std::string pattern = std::get<SqlVarcharType>(rhs);

    size_t p = 0;

    for (int i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == '%') continue;

        if (i == 0 || pattern[i - 1] == '%') {
            while (p < str.size() && str[p] != pattern[i]) {
                ++p;
            }

            if (p == str.size()) {
                return false;
            }
        } else {
            if (p == str.size() || str[p] != pattern[i]) {
                return false;
            }
        }

        p++;
    }

    return true;
}

//==========================================JOIN-HANDLE===================================================
void JoinHandle::addToJoinedTuple(JoinedTuple &theJoinTuple, const std::string &entityName, const Tuple &aTuple) {
    for (auto &element: aTuple) {
        auto key = std::make_pair(entityName, element.first);
        theJoinTuple.insert(std::make_pair(key, element.second));
    }
}

JoinedTuple JoinHandle::covertTupleToJoinedTuple(const std::string &entityName, const Tuple &aTuple) {
    JoinedTuple theJoinedTuple;
    addToJoinedTuple(theJoinedTuple, entityName, aTuple);
    return theJoinedTuple;
}

void JoinHandle::removeFromJoinedTuple(JoinedTuple &theJoinTuple, const std::string &entityName, const Tuple &aTuple) {
    for (auto &element: aTuple) {
        auto key = std::make_pair(entityName, element.first);
        if (theJoinTuple.contains(key)) {
            theJoinTuple.erase(key);
        }
    }
}

std::vector<JoinedTuple> JoinHandle::joinEntity(const JoinExpression &joinExpression, std::vector<JoinedTuple> &left,
                                                const std::vector<Tuple> &right, JoinedTuple &nullLeftTuple,
                                                const Tuple &nullRightTuple) {
    std::vector<JoinedTuple> result;

    std::map<int, bool> noteLeft, noteRight;

    for (int i = 0; i < left.size(); ++i) {
        for (int j = 0; j < right.size(); ++j) {
            addToJoinedTuple(left[i], joinExpression.targetEntityName, right[j]);
            if (joinExpression.apply(left[i])) {
                noteLeft[i] = true;
                noteRight[j] = true;
                result.push_back(left[i]);
            }
            removeFromJoinedTuple(left[i], joinExpression.targetEntityName, right[j]);
        }
    }

    if (joinExpression.joinType == JoinTypes::left) {
        for (int i = 0; i < left.size(); ++i) {
            if (!noteLeft.contains(i)) {
                addToJoinedTuple(left[i], joinExpression.targetEntityName, nullRightTuple);
                result.push_back(left[i]);
                removeFromJoinedTuple(left[i], joinExpression.targetEntityName, nullRightTuple);
            }
        }
    }

    if (joinExpression.joinType == JoinTypes::right) {
        for (int j = 0; j < right.size(); ++j) {
            if (!noteRight.contains(j)) {
                addToJoinedTuple(nullLeftTuple, joinExpression.targetEntityName, right[j]);
                result.push_back(nullLeftTuple);
                removeFromJoinedTuple(nullLeftTuple, joinExpression.targetEntityName, right[j]);
            }
        }
    }

    return result;
}

//==========================================TUPLE-HANDLE===================================================
Tuple TupleHandle::baseInternalBNode(const SharedPagePtr &aPage) {
    Tuple theTuple;
    theTuple.insert(std::make_pair(SpecialKey::BTREE_KEY, aPage->get_btree_key()));
    theTuple.insert(std::make_pair(SpecialKey::CHILD_BTREE_KEY, SqlIntType(aPage->pageIndex)));
    return theTuple;
}

Tuple TupleHandle::baseInternalBNode(const BPlusKey &key, const uint32_t &childIndex) {
    Tuple theTuple;
    theTuple.insert(std::make_pair(SpecialKey::BTREE_KEY, key));
    theTuple.insert(std::make_pair(SpecialKey::CHILD_BTREE_KEY, SqlIntType(childIndex)));
    return theTuple;
}

BPlusKey TupleHandle::genBNodeKey(const Tuple &aTuple, const IndexingMetadata &indexingMetadata) {
    BPlusKey key;
    for (auto &element: indexingMetadata.keys) {
        key.push_back(SqlTypeHandle::convertToPairKeyElement(aTuple.at(element)));
    }
    return key;
}

void TupleHandle::addBNodeKey(Tuple &aTuple, const IndexingMetadata &indexingMetadata) {
    aTuple.insert(std::make_pair(SpecialKey::BTREE_KEY, genBNodeKey(aTuple, indexingMetadata)));
}

Tuple TupleHandle::makeSecondaryTuple(const Tuple &aTuple, const IndexingMetadata &indexingMetadata,
                                      const std::string &primaryKey) {
    Tuple result;
    result.insert(std::make_pair(SpecialKey::BTREE_KEY, genBNodeKey(aTuple, indexingMetadata)));
    result.insert(std::make_pair(primaryKey, aTuple.at(primaryKey)));
    return result;
}

BPlusKey TupleHandle::getBTreeKey(const Tuple &tuple) {
    return SqlTypeHandle::concretizeSqlType<BPlusKey>(tuple.at(SpecialKey::BTREE_KEY));
}

uint32_t TupleHandle::getBtreeChild(const Tuple &tuple) {
    return SqlTypeHandle::concretizeSqlType<SqlIntType>(tuple.at(SpecialKey::CHILD_BTREE_KEY));
}

//===========================================FOLDER-HANDLE===================================================
bool FolderHandle::containFolder(const std::string &theFolder, const std::string &subFolder) {
    return std::filesystem::exists(theFolder + "/" + subFolder);
}

bool FolderHandle::createIfNotExist(const std::string &theFolder, const std::string &pFolder) {
    std::string dir = (pFolder == "" ? "" : pFolder + "/") + theFolder;
    if (!std::filesystem::exists(dir)) {
        return std::filesystem::create_directory(dir);
    }
    return false;
}

bool FolderHandle::deleteIfExist(const std::string &theFolder, const std::string &pFolder) {
    std::string dir = (pFolder == "" ? "" : pFolder + "/") + theFolder;
    if (std::filesystem::exists(dir)) {
        std::filesystem::remove_all(dir);
        return true;
    }
    return false;
}

void FolderHandle::eachFolder(const std::string &theFolder, std::function<void(const std::string &)> aCallback) {
    for (const auto &entry: std::filesystem::directory_iterator(theFolder)) {
        aCallback(entry.path().filename().string());
    }
}
