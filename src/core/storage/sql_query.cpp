//
// Created by chiendd on 08/09/2024.
//

#include "sql_query.h"

namespace Csql {
    void SQLQuery::setEntityName(std::string anEntityName) {
        entityName = std::move(anEntityName);
    }

    void SQLQuery::addTaget(std::string entityName, std::string attributeName) {
        targets.emplace_back(std::move(entityName), std::move(attributeName));
    }

    void SQLQuery::setWhereExpression(const WhereExpression &aWhereExpression) {
        whereExpression = aWhereExpression;
    }

    void SQLQuery::addGroupCondition(std::string entityName, std::string attributeName) {
        groupConditions.emplace_back(std::move(entityName), std::move(attributeName));
        isGroupBy = true;
    }

    void SQLQuery::addOrderCondition(std::string entityName, std::string attributeName, OrderType orderType) {
        orderConditions.emplace_back(std::make_pair(std::move(entityName), std::move(attributeName)), orderType);
        isOrderBy = true;
    }

    void SQLQuery::setLimit(int limit, int offset) {
        limitCondition = std::make_pair(limit, offset);
        isLimit = true;
    }

}