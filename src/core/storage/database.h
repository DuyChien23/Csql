//
// Created by chiendd on 21/07/2024.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include "entity.h"
#include "sql_query.h"
#include "storage.h"

namespace Csql {
    class Database : public Storage {
    public:
        Database(std::string databaseName) : Storage(databaseName) {};
        ~Database() {};

        void createTable(std::ostream& anOutput, SharedEntityPtr& anEntityPtr);
        void showTables(std::ostream& anOutput);
        void describeTable(std::ostream& anOutput, std::string aEntityName);
        void dropTable(std::ostream& anOutput, std::string aEntityName);
        void insert(
        std::ostream &anOutput,
        std::string aEntityName,
        std::vector<std::pair<std::string, std::string>> &aValues,
        bool hasAtrributeName = false);
        void select(std::ostream& anOutput, SQLQueryPtr& aSelectQuery);
        void deleteTuples(std::ostream& anOutput, const SQLQueryPtr& aDeleteQuery);

    private:
        void validateCreateTable(const SharedEntityPtr& anEntityPtr);
        void validateInsert(std::ostream &anOutput, const SharedEntityPtr& theEntity, const Tuple &aTuple);
        void doInsert(std::ostream &anOutput, std::string aEntityName, Tuple &theTuple);
        void validateTableExisted(std::string aEntityName);
        void validateTableNotExisted(std::string aEntityName);
    };

    using DatabasePtr  = Database*;
    using UniqueDatabasePtr = std::unique_ptr<Database>;
}

#endif //DATABASE_H
