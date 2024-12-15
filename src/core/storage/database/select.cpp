//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../util/helpers.h"
#include "../../util/errors.h"
#include "../../view/query_result_view.h"
#include "../../view/table_view.h"

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
                aSelectQuery->addTaget(entityName, anAttribute->getName());
            });
        };
        addTargetFromEntity(theEntityName);
        for (auto &joinExpression: aSelectQuery->getListJoin()) {
            addTargetFromEntity(joinExpression.targetEntityName);
        }
    }

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

    //=================TABLE-VIEW=======================================================
    TableView theTableView(aSelectQuery->headers);

    for (auto &row: result) {
        std::vector<std::string> data;
        for (auto &target: aSelectQuery->getTargets()) {
            data.push_back(Helpers::SqlTypeHandle::covertSqlTypeToString(row[target]));
        }
        theTableView.addData(data);
    }

    theTableView.show(anOutput);
}