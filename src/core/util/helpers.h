//
// Created by chiendd on 05/09/2024.
//

#ifndef HELPERS_H
#define HELPERS_H
#include "types.h"

#endif //HELPERS_H

namespace Csql {
   enum class SqlTypeCompareResult {
      cant_compare,
      less_than,
      equal,
      greater,
   };

   class Helpers {
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
            throw Errors(expressionError);
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
}