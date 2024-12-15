//
// Created by chiendd on 21/07/2024.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include "../sql_query.h"
#include "../storage.h"
#include "../../controller/concurrency/lock_manager.h"

#define LOCK_TABLE_SHARED(tableName) if (!LockManagerInstance::get(name)->lockTable(transactionPtr.get(), LockMode::SHARED, tableName)) \
    throw Errors("Can't lock table " + tableName + " in shared mode")
#define LOCK_TABLE_EXCLUSIVE(tableName) if (!LockManagerInstance::get(name)->lockTable(transactionPtr.get(), LockMode::EXCLUSIVE, tableName)) \
    throw Errors("Can't lock table " + tableName + " in exclusive mode")

#define UNLOCK_TABLE(tableName) LockManagerInstance::get(name)->unlockTable(transactionPtr.get(), tableName)

enum class ConditionType {
    equal_condition,
    greater_than_condition,
    less_than_condition,
    between_condition,
};

struct Condition {
    std::string attributeName;
    ConditionType conditionType;
    std::vector<SqlTypes> values;
};

class Database : public Storage {
public:
    Database(std::string databaseName) : Storage(databaseName) {
         transactionPtr = nullptr;
    };

    ~Database() = default;

    void createTable(std::ostream &anOutput, SharedEntityPtr &anEntityPtr);

    void showTables(std::ostream &anOutput);

    void describeTable(std::ostream &anOutput, std::string aEntityName);

    void dropTable(std::ostream &anOutput, std::string aEntityName);

    void insert(
        std::ostream &anOutput,
        std::string aEntityName,
        std::vector<std::pair<std::string, std::string> > &aValues,
        bool hasAtrributeName = false);

    void select(std::ostream &anOutput, SQLQueryPtr &aSelectQuery);

    void deleteTuples(std::ostream &anOutput, const SQLQueryPtr &aDeleteQuery);

    void update(std::ostream &anOutput, const UpdateQueryPtr &aUpdateQuery);

    void beginTransaction();

    void abort();

    void commit();

    void undo();

    TransactionPtr transactionPtr = nullptr;
private:
    void validateCreateTable(const SharedEntityPtr &anEntityPtr);

    void validateInsert(std::ostream &anOutput, const SharedEntityPtr &theEntity, const Tuple &aTuple);

    void doInsert(std::ostream &anOutput, std::string aEntityName, Tuple &theTuple);

    void validateTableExisted(std::string aEntityName);

    void validateTableNotExisted(std::string aEntityName);

    std::vector<Tuple> getRowsByCondition(const std::string &entityName, std::vector<Condition> &conditions);
};

using DatabasePtr = Database *;
using UniqueDatabasePtr = std::unique_ptr<Database>;


#endif //DATABASE_H
