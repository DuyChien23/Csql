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

#endif //HELPERS_H

namespace Csql {
   enum class SqlTypeCompareResult {
      cant_compare,
      less_than,
      equal,
      greater,
   };

   namespace Helpers {
      //============================SQL-TYPEs=============================================================
       class SqlTypeHandle {
          template<typename T, typename V>
          struct variant_index;

          template<typename T, typename First, typename... Rest>
          struct variant_index<T, std::variant<First, Rest...>> {
             static constexpr int value = std::is_same_v<T, First> ? 0 : variant_index<T, std::variant<Rest...>>::value + 1;
          };

          template<typename T>
          struct variant_index<T, std::variant<>> {
             static constexpr int value = 0;
          };

          template<typename T>
          static constexpr int sql_types_index_v = variant_index<T, SqlTypes>::value;

       public:
          static std::string covertSqlTypeToString(const SqlTypes& value) {
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

          static DataTypes covertStringToDataType(std::string type) {
             if (type == "bool") return DataTypes::bool_type;
             if (type == "date") return DataTypes::datetime_type;
             if (type == "int") return DataTypes::int_type;
             if (type == "integer") return DataTypes::int_type;
             if (type == "float") return DataTypes::float_type;
             if (type == "varchar") return DataTypes::varchar_type;
             return DataTypes::null_type;
          }

          static uint32_t sizeOfSqlType(const SqlTypes& value) {
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
                default:
                   return 0;
             }
          }

          static uint32_t sizeOfTuple(const Tuple& aTuple) {
             uint32_t result = 0;
             for (auto& element : aTuple) {
                result += sizeof(uint32_t);
                result += element.first.size();
                result += sizeOfSqlType(element.second);
             }
             return result;
          }

          static std::string covertDataTypeToString(const DataTypes& type) {
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
      };

      //============================EXPRESSION============================================================
      class ExpressionHandle {
      public:
         static SqlTypeCompareResult compareSqlType(const SqlTypes& lhs,const SqlTypes& rhs) {
            if (lhs.index() != rhs.index()) return SqlTypeCompareResult::cant_compare;

            if (std::holds_alternative<SqlBoolType>(lhs) || std::holds_alternative<SqlNullType>(lhs) || std::holds_alternative<SqlVarcharType>(lhs)) {
               return SqlTypeCompareResult::cant_compare;
            }

            if (lhs == rhs) return SqlTypeCompareResult::equal;

            return lhs < rhs ? SqlTypeCompareResult::less_than : SqlTypeCompareResult::greater;
         }

         static bool likeMatch(const SqlTypes& lhs, const SqlTypes& rhs) {
            if (!std::holds_alternative<SqlVarcharType>(lhs) || !std::holds_alternative<SqlVarcharType>(rhs)) {
               throw Errors("Type of operand invalid");
            }

            std::string str = std::get<SqlVarcharType>(lhs);
            std::string pattern = std::get<SqlVarcharType>(lhs);

            size_t s = 0;
            size_t p = 0;
            size_t star = std::string::npos;
            size_t match = 0;

            while (s < str.size()) {
               if (p < pattern.size() && (pattern[p] == '_' || pattern[p] == str[s])) {
                  ++s;
                  ++p;
               } else if (p < pattern.size() && pattern[p] == '%') {
                  star = p++;
                  match = s;
               } else if (star != std::string::npos) {
                  p = star + 1;
                  s = ++match;
               } else {
                  return false;
               }
            }

            while (p < pattern.size() && pattern[p] == '%') {
               ++p;
            }

            return p == pattern.size();
         }
      };

      //=================================JOIN_TUPLE===================================================================//

      class JoinHandle {
      public:
         static void addToJoinedTuple(JoinedTuple& theJoinTuple, const std::string& entityName, const Tuple& aTuple) {
            for (auto& element : aTuple) {
               theJoinTuple.insert(entityName, element.first, element.second);
            }
         }

         static JoinedTuple covertTupleToJoinedTuple(const std::string& entityName, const Tuple& aTuple) {
            JoinedTuple theJoinedTuple;
            addToJoinedTuple(theJoinedTuple, entityName, aTuple);
            return theJoinedTuple;
         }

         static void removeFromJoinedTuple(JoinedTuple& theJoinTuple, const std::string& entityName, const Tuple& aTuple) {
            for (auto& element : aTuple) {
               if (theJoinTuple.contains(entityName, element.first)) {
                  theJoinTuple.erase(entityName, element.first);
               }
            }
         }

         static std::vector<JoinedTuple> joinEntity(
            const JoinExpression& joinExpression,
            std::vector<JoinedTuple>& left,
            const std::vector<Tuple>& right,
            JoinedTuple& nullLeftTuple,
            const Tuple& nullRightTuple)
         {
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
      };

      class FolderHandle {
      public:
         static bool containFolder(const std::string& theFolder, const std::string& subFolder) {
            return std::filesystem::exists(theFolder + "/" + subFolder);
         }

         static bool createIfNotExist(const std::string& theFolder, const std::string& pFolder = "") {
            if (!std::filesystem::exists(theFolder)) {
               return std::filesystem::create_directory((pFolder == "" ? "" : pFolder + "/") + theFolder);
            }
            return false;
         }

         static void eachFolder(const std::string& theFolder, std::function<void(const std::string&)> aCallback) {
            for (const auto& entry : std::filesystem::directory_iterator(theFolder)) {
               aCallback(entry.path().filename().string());
            }
         }
      };
   };
}