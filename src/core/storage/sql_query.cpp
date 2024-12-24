//
// Created by chiendd on 08/09/2024.
//

#include "sql_query.h"

#include "../util/errors.h"
#include "expression/aggregate_expression/normal_expression.h"

void SQLQuery::setEntityName(std::string anEntityName) {
    entityName = std::move(anEntityName);
}

void SQLQuery::addTarget(TargetExpression* tExp, const std::string& columnName) {
    targets.emplace_back(tExp);
    isSelectedAll = false;
    isAggregated = true;
    addHeader(columnName);
}

void SQLQuery::addTarget(const std::string& entityName, const std::string& attributeName) {
    targets.emplace_back(new NormalExpression(entityName, attributeName));
    isSelectedAll = false;
    addHeader(entityName + "." + attributeName);
}

void SQLQuery::addHeader(std::string columnName) {
    for (auto &h : headers) {
        if (h == columnName) {
            throw Errors("Column name is duplicated");
        }
    }
    headers.push_back(columnName);
}


void SQLQuery::setWhereExpression(const WhereExpression &aWhereExpression) {
    whereExpression = aWhereExpression;
}

void SQLQuery::addGroupCondition(std::string entityName, std::string attributeName) {
    groupConditions.emplace_back(entityName.empty() ? this->entityName : std::move(entityName), std::move(attributeName));
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

std::vector<TargetExpression*> &SQLQuery::getTargets() {
    return targets;
}

TargetExpression *SQLQuery::getTarget(const std::string &columnName) {
    for (int i = 0; i < headers.size(); ++i) {
        if (headers[i] == columnName) {
            return targets[i];
        }
    }

    throw Errors("Column name not found");
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


void UpdateQuery::setEntityName(std::string anEntityName) {
    entityName = std::move(anEntityName);
}

void UpdateQuery::addUpdate(std::string anAttributeName, Expression *anExpression) {
    updatesList.emplace_back(std::move(anAttributeName), anExpression);
}


void UpdateQuery::setWhereExpression(const WhereExpression &aWhereExpression) {
    whereExpression = aWhereExpression;
}

std::string UpdateQuery::getEntityName() {
    return entityName;
}

std::vector<std::pair<std::string, Expression *>> &UpdateQuery::getUpdatesList() {
    return updatesList;
}

WhereExpression &UpdateQuery::getWhereExpression() {
    return whereExpression;
}




