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
#include "expression/aggregate_expression/target_expression.h"


enum class OrderType {
    asc, desc
};

class SQLQuery {
public:
    SQLQuery() = default;

    void setEntityName(std::string anEntityName);

    void addTarget(TargetExpression *tExp, const std::string& columnName);
    void addTarget(const std::string& entityName, const std::string& attributeName);
    void addHeader(std::string columnName);

    void setWhereExpression(const WhereExpression &aWhereExpression);

    void addGroupCondition(std::string entityName, std::string attributeName);

    void addOrderCondition(std::string entityName, std::string attributeName, OrderType orderType);

    void setLimit(int limit, int offset = 0);

    void addJoinExpression(const JoinExpression &joinExpression);

    std::string getEntityName();

    std::vector<TargetExpression*> &getTargets();
    TargetExpression* getTarget(const std::string &columnName);

    WhereExpression &getWhereExpression();

    std::vector<std::pair<std::string, std::string> > &getGroupConditions();

    std::vector<std::pair<std::pair<std::string, std::string>, OrderType> > &getOrderConditions();

    std::pair<int, int> getLimitCondition();

    std::vector<JoinExpression> &getListJoin();

    bool isSelectedAll = true;
    bool isGroupBy = false;
    bool isOrderBy = false;
    bool isLimit = false;
    bool isJoined = false;
    bool isAggregated = false;

    std::vector<std::string> headers;

protected:
    std::string entityName;
    std::vector<TargetExpression*> targets;
    WhereExpression whereExpression;
    std::vector<std::pair<std::string, std::string> > groupConditions;
    std::vector<std::pair<std::pair<std::string, std::string>, OrderType> > orderConditions;
    std::pair<int, int> limitCondition = {1000000000, 0};
    std::vector<JoinExpression> listJoin;
};

class UpdateQuery {
public:
    UpdateQuery() = default;
    void setEntityName(std::string anEntityName);
    void addUpdate(std::string anAttributeName, Expression* anExpression);
    void setWhereExpression(const WhereExpression &aWhereExpression);
    std::string getEntityName();
    std::vector<std::pair<std::string, Expression*>> &getUpdatesList();
    WhereExpression &getWhereExpression();

protected:
    std::string entityName;
    std::vector<std::pair<std::string, Expression*>> updatesList;
    WhereExpression whereExpression;
};

using SQLQueryPtr = std::unique_ptr<SQLQuery>;
using UpdateQueryPtr = std::unique_ptr<UpdateQuery>;


#endif //SQL_QUERY_H
