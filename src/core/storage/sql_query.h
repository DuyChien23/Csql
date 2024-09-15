//
// Created by chiendd on 08/09/2024.
//

#ifndef SQL_QUERY_H
#define SQL_QUERY_H
#include <memory>
#include <string>
#include <vector>

#include "expression/join_expression.h"
#include "expression/where_expression.h"

namespace Csql {
    enum class OrderType {
        ASC, DESC
    };

    class SQLQuery {
    public:
        SQLQuery() = default;

        void setEntityName(std::string anEntityName);
        void addTaget(std::string entityName, std::string attributeName, std::string columnName = "");
        void setWhereExpression(const WhereExpression& aWhereExpression);
        void addGroupCondition(std::string entityName, std::string attributeName);
        void addOrderCondition(std::string entityName, std::string attributeName, OrderType orderType);
        void setLimit(int limit, int offset = 0);
        void addJoinExpression(const JoinExpression& joinExpression);

        std::string getEntityName();
        std::vector<std::pair<std::string, std::string>>& getTargets();
        WhereExpression& getWhereExpression();
        std::vector<std::pair<std::string, std::string>>& getGroupConditions();
        std::vector<std::pair<std::pair<std::string, std::string>, OrderType>>& getOrderConditions();
        std::pair<int, int> getLimitCondition();
        std::vector<JoinExpression>& getListJoin();

        bool isSelectedAll  = true;
        bool isGroupBy      = false;
        bool isOrderBy      = false;
        bool isLimit        = false;
        bool isJoined       = false;

        std::vector<std::string> headers;

    protected:
        std::string                                                             entityName;
        std::vector<std::pair<std::string, std::string>>                        targets;
        WhereExpression                                                         whereExpression;
        std::vector<std::pair<std::string, std::string>>                        groupConditions;
        std::vector<std::pair<std::pair<std::string, std::string>, OrderType>>  orderConditions;
        std::pair<int, int>                                                     limitCondition;
        std::vector<JoinExpression>                                             listJoin;
    };

    using SQLQueryPtr = std::unique_ptr<SQLQuery>;
}

#endif //SQL_QUERY_H
