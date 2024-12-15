//
// Created by chiendd on 05/09/2024.
//

#ifndef HELPERS_H
#define HELPERS_H
#include <vector>
#include <filesystem>
#include <functional>

#include "errors.h"
#include "../storage/expression/join_expression.h"
#include "../storage/page/slotted_page.h"

#endif //HELPERS_H


enum class SqlTypeCompareResult {
    cant_compare,
    less_than,
    equal,
    greater,
    not_equal,
};

namespace Helpers {
    //============================SQL-TYPEs=============================================================
    class SqlTypeHandle {
    public:
        template<typename T>
        static T concretizeSqlType(const SqlTypes &value) {
            return std::get<T>(value);
        }

        static std::string covertSqlTypeToString(const SqlTypes &value);

        static DataTypes covertSqlValueToDataTypes(const SqlTypes &value);

        static DataTypes covertStringToDataType(std::string type);

        static SqlTypes covertStringToSqlType(DataTypes type, std::string value);

        static uint32_t sizeOfSqlType(const SqlTypes &value);

        static uint32_t sizeOfTuple(const Tuple &aTuple);

        static std::string covertDataTypeToString(const DataTypes &type);

        static PairKeyElement convertToPairKeyElement(const SqlTypes &value);

    private:
        template<typename T, typename V>
        struct variant_index;

        template<typename T, typename First, typename... Rest>
        struct variant_index<T, std::variant<First, Rest...> > {
            static constexpr int value = std::is_same_v<T, First>
                                             ? 0
                                             : variant_index<T, std::variant<Rest...> >::value + 1;
        };

        template<typename T>
        struct variant_index<T, std::variant<> > {
            static constexpr int value = 0;
        };

        template<typename T>
        static constexpr int sql_types_index_v = variant_index<T, SqlTypes>::value;
    };

    //============================EXPRESSION============================================================
    class ExpressionHandle {
    public:
        static SqlTypeCompareResult compareSqlType(const SqlTypes &lhs, const SqlTypes &rhs);

        static bool likeMatch(const SqlTypes &lhs, const SqlTypes &rhs);
    };

    //=================================JOIN_TUPLE===================================================================//

    class JoinHandle {
    public:
        static void addToJoinedTuple(JoinedTuple &theJoinTuple, const std::string &entityName, const Tuple &aTuple);

        static JoinedTuple covertTupleToJoinedTuple(const std::string &entityName, const Tuple &aTuple);

        static void removeFromJoinedTuple(JoinedTuple &theJoinTuple, const std::string &entityName,
                                          const Tuple &aTuple);

        static std::vector<JoinedTuple> joinEntity(
            const JoinExpression &joinExpression,
            std::vector<JoinedTuple> &left,
            const std::vector<Tuple> &right,
            JoinedTuple &nullLeftTuple,
            const Tuple &nullRightTuple);
    };

    //=============================================TUPLE=========================================================//
    class TupleHandle {
    public:
        static Tuple baseInternalBNode(const SharedPagePtr &aPage);

        static Tuple baseInternalBNode(const BPlusKey &key, const uint32_t &childIndex);

        static BPlusKey genBNodeKey(const Tuple &aTuple, const IndexingMetadata &indexingMetadata);

        static void addBNodeKey(Tuple &aTuple, const IndexingMetadata &indexingMetadata);

        static Tuple makeSecondaryTuple(const Tuple &aTuple, const IndexingMetadata &indexingMetadata,
                                        const std::string &primaryKey);

        static BPlusKey getBTreeKey(const Tuple &tuple);

        static uint32_t getBtreeChild(const Tuple &tuple);
    };

    class FolderHandle {
    public:
        static bool containFolder(const std::string &theFolder, const std::string &subFolder);

        static bool createIfNotExist(const std::string &theFolder, const std::string &pFolder = "");

        static bool deleteIfExist(const std::string &theFolder, const std::string &pFolder = "");

        static void eachFolder(const std::string &theFolder, std::function<void(const std::string &)> aCallback);
    };
};
