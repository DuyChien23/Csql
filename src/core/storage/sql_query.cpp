//
// Created by chiendd on 08/09/2024.
//

#include "sql_query.h"

void SQLQuery::setEntityName(std::string anEntityName) {
    entityName = std::move(anEntityName);
}

void SQLQuery::addTaget(std::string entityName, std::string attributeName, std::string columnName) {
    if (columnName.empty()) {
        columnName = (entityName.empty() ? "" : entityName + ".") + attributeName;
    }

    targets.emplace_back(std::move(entityName), std::move(attributeName));
    headers.push_back(columnName);
    isSelectedAll = false;
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

void SQLQuery::addJoinExpression(const JoinExpression &joinExpression) {
    listJoin.push_back(joinExpression);
    isJoined = true;
}

std::string SQLQuery::getEntityName() {
    return entityName;
}

std::vector<std::pair<std::string, std::string> > &SQLQuery::getTargets() {
    return targets;
}

std::vector<std::pair<std::string, std::string> > &SQLQuery::getGroupConditions() {
    return groupConditions;
}

WhereExpression &SQLQuery::getWhereExpression() {
    return whereExpression;
}

std::vector<std::pair<std::pair<std::string, std::string>, OrderType> > &SQLQuery::getOrderConditions() {
    return orderConditions;
}

std::pair<int, int> SQLQuery::getLimitCondition() {
    return limitCondition;
}

std::vector<JoinExpression> &SQLQuery::getListJoin() {
    return listJoin;
}
