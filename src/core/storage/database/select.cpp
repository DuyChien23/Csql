//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../util/helpers.h"
#include "../../util/errors.h"
#include "../../view/query_result_view.h"
#include "../../view/table_view.h"

namespace std {
    template <>
    struct hash<SqlTypes> {
        std::size_t operator()(const SqlTypes& t) const {
            return std::visit([](auto&& arg) {
                return std::hash<std::decay_t<decltype(arg)>>{}(arg);  // Băm từng kiểu trong variant
            }, t);
        }
    };


    template <>
    struct hash<std::vector<SqlTypes>> {
        std::size_t operator()(const std::vector<SqlTypes>& v) const {
            std::size_t seed = 0;
            for (const auto& elem : v) {
                seed ^= std::hash<SqlTypes>{}(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}

void Database::select(std::ostream &anOutput, SQLQueryPtr &aSelectQuery) {
    LOCK_TABLE_SHARED(aSelectQuery->getEntityName());
    for (auto &joinExpression: aSelectQuery->getListJoin()) {
        LOCK_TABLE_SHARED(joinExpression.targetEntityName);
    }

    std::string theEntityName = aSelectQuery->getEntityName();
    validateTableExisted(theEntityName);

    SharedEntityPtr theEntity = getEntity(theEntityName);

    //=====================TARGET==================================================
    if (aSelectQuery->isSelectedAll) {
        auto addTargetFromEntity = [&](std::string entityName) {
            validateTableExisted(entityName);
            getEntity(entityName)->eachAttribute([&](Attribute *anAttribute) {
                aSelectQuery->addTarget(entityName, anAttribute->getName());
            });
        };
        addTargetFromEntity(theEntityName);
        for (auto &joinExpression: aSelectQuery->getListJoin()) {
            addTargetFromEntity(joinExpression.targetEntityName);
        }
    }

    TableView theTableView(aSelectQuery->headers);

    //=====================MAIN=TABLE==================================================
    std::vector<Condition> conditions(0);
    std::vector<JoinedTuple> result;
    for (auto row : getRowsByCondition(theEntityName, conditions)) {
        result.push_back(Helpers::JoinHandle::covertTupleToJoinedTuple(theEntityName, row));
    }

    //=====================JOIN====================================================
    JoinedTuple currentNull = Helpers::JoinHandle::covertTupleToJoinedTuple(theEntityName, theEntity->baseNullTuple());
    for (auto &joinExpression: aSelectQuery->getListJoin()) {
        validateTableExisted(joinExpression.targetEntityName);
        auto other = getRowsByCondition(joinExpression.targetEntityName, conditions);
        auto temp = result;
        result.resize(0);

        switch (joinExpression.joinType) {
            case JoinTypes::inner: {
                for (auto row1 : temp) {
                    for (auto row2 : other) {
                        auto row = row1;
                        Helpers::JoinHandle::addToJoinedTuple(row, joinExpression.targetEntityName, row2);
                        if (joinExpression.apply(row)) {
                            result.push_back(row);
                        }
                    }
                }
                break;
            }
            case JoinTypes::cross: {
                for (auto row1 : temp) {
                    for (auto row2 : other) {
                        auto row = row1;
                        Helpers::JoinHandle::addToJoinedTuple(row, joinExpression.targetEntityName, row2);
                        result.push_back(row);
                    }
                }
                break;
            }
            case JoinTypes::left: {
                for (auto row1 : temp) {
                    bool flag = false;
                    for (auto row2 : other) {
                        auto row = row1;
                        Helpers::JoinHandle::addToJoinedTuple(row, joinExpression.targetEntityName, row2);
                        if (joinExpression.apply(row)) {
                            result.push_back(row);
                            flag = true;
                        }
                    }
                    if (!flag) {
                        Helpers::JoinHandle::addToJoinedTuple(row1, joinExpression.targetEntityName, getEntity(joinExpression.targetEntityName)->baseNullTuple());
                        result.push_back(row1);
                    }
                }
                break;
            }
            case JoinTypes::right: {
                for (auto row2 : other) {
                    bool flag = false;
                    for (auto row1 : temp) {
                        auto row = row1;
                        Helpers::JoinHandle::addToJoinedTuple(row, joinExpression.targetEntityName, row2);
                        if (joinExpression.apply(row)) {
                            result.push_back(row);
                            flag = true;
                        }
                    }
                    if (!flag) {
                        auto row = currentNull;
                        Helpers::JoinHandle::addToJoinedTuple(row, joinExpression.targetEntityName, row2);
                        result.push_back(row);
                    }
                }

                break;
            }
            case JoinTypes::full: {
                //TODO:
                break;
            }
            default: {
                throw Errors("Join type not found");
            }
        }

        Helpers::JoinHandle::addToJoinedTuple(currentNull, joinExpression.targetEntityName, getEntity(joinExpression.targetEntityName)->baseNullTuple());
    }

    //=================WHERE===========================================================
    std::vector<JoinedTuple> temp = result;
    result.resize(0);
    for (auto &row: temp) {
        if (aSelectQuery->getWhereExpression().apply(row)) {
            result.push_back(row);
        }
    }

    //=================GROUP-BY=======================================================
    std::vector<std::vector<SqlTypes>> datas;

    if (aSelectQuery->getGroupConditions().empty()) {
        if (aSelectQuery->isAggregated) {
            std::vector<SqlTypes> data;
            for (auto &target: aSelectQuery->getTargets()) {
                data.push_back(target->apply(result));
            }
            datas.push_back(data);
        } else {
            for (auto &row : result) {
                std::vector<SqlTypes> data;
                for (auto &target: aSelectQuery->getTargets()) {
                    data.push_back(target->apply({row}));
                }
                datas.push_back(data);
            }
        }
    } else {
        std::unordered_map<std::vector<SqlTypes>, std::vector<JoinedTuple>> groupMap;
        for (auto &row: result) {
            std::vector<SqlTypes> group;
            for (auto &groupCondition: aSelectQuery->getGroupConditions()) {
                group.push_back(row[{groupCondition.first, groupCondition.second}]);
            }
            groupMap[group].push_back(row);
        }

        for (auto &[key, value]: groupMap) {
            std::vector<SqlTypes> data;
            for (auto &target: aSelectQuery->getTargets()) {
                data.push_back(target->apply(value));
            }
            datas.push_back(data);
        }
    }

    //=================ORDER-BY=======================================================
    if (!aSelectQuery->getOrderConditions().empty()) {
        std::vector<int> ids;
        bool flag = false;
        for (auto &[t, _]: aSelectQuery->getOrderConditions()) {
            for (int i = 0; i < aSelectQuery->headers.size(); ++i) {
                auto &header = aSelectQuery->headers.at(i);
                if (!t.first.empty()) {
                    if (header == t.first + "." + t.second) {
                        ids.push_back(i);
                        flag = true;
                        break;
                    }
                } else {
                    auto getAttributeName = [&](const std::string& header) {
                        std::string ans;
                        for (auto c : header) {
                            if (c == '.') {
                                ans = "";
                            } else {
                                ans += c;
                            }
                        }
                        return ans;
                    };
                    if (header == t.second || getAttributeName(header) == t.second) {
                        ids.push_back(i);
                        flag = true;
                        break;
                    }
                }
            }

            if (!flag) {
                throw Errors("Column name not found");
            }
        }

        std::sort(datas.begin(), datas.end(), [&](std::vector<SqlTypes> &a, std::vector<SqlTypes> &b) {
            for (int i = 0; i < ids.size(); ++i) {
                auto &id = ids[i];
                auto orderType = aSelectQuery->getOrderConditions().at(i).second;

                if (a[id] != b[id]) {
                    if (orderType == OrderType::asc) {
                        return a[id] < b[id];
                    } else {
                        return a[id] > b[id];
                    }
                }
            }

            return false;
        });
    }

    //=================LIMIT===========================================================
    int start = aSelectQuery->getLimitCondition().second;
    int end = std::min(int(datas.size()), start + aSelectQuery->getLimitCondition().first);


    for (; start < end; ++start) {
        std::vector<std::string> data;
        for (auto v : datas[start]) {
            data.push_back(Helpers::SqlTypeHandle::covertSqlTypeToString(v));
        }
        theTableView.addData(data);
    }

    theTableView.show(anOutput);
}

