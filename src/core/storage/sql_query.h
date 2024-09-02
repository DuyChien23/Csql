//
// Created by chiendd on 08/09/2024.
//

#ifndef SQL_QUERY_H
#define SQL_QUERY_H
#include <string>
#include <vector>

#include "expression/where_expression.h"

namespace Csql {
    enum class OrderType {
        ASC, DESC
    };

    class SQLQuery {
    public:
        SQLQuery() {}
        void setEntityName(std::string anEntityName);
        void addTaget(std::string entityName, std::string attributeName);
        void setWhereExpression(const WhereExpression& aWhereExpression);
        void addGroupCondition(std::string entityName, std::string attributeName);
        void addOrderCondition(std::string entityName, std::string attributeName, OrderType orderType);
        void setLimit(int limit, int offset = 0);

        bool isGroupBy  = false;
        bool isOrderBy  = false;
        bool isLimit    = false;

    protected:
        std::string                                                             entityName;
        std::vector<std::pair<std::string, std::string>>                        targets;
        WhereExpression                                                         whereExpression;
        std::vector<std::pair<std::string, std::string>>                        groupConditions;
        std::vector<std::pair<std::pair<std::string, std::string>, OrderType>>  orderConditions;
        std::pair<int, int>                                                     limitCondition;
    };
}

#endif //SQL_QUERY_H
