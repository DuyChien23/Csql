//
// Created by chiendd on 21/07/2024.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include "entity.h"
#include "storage.h"

namespace Csql {
    class Database;

    class Database : public Storage {
    public:
        Database(std::string databaseName) : Storage(databaseName) {};
        ~Database() {};

        void createTable(std::ostream& anOutput, SharedEntityPtr& anEntityPtr);
        void showTables(std::ostream& anOutput);
        void describeTable(std::ostream& anOutput, std::string aEntityName);
        void dropTable(std::ostream& anOutput, std::string aEntityName);
        void insert(std::ostream& anOutput, std::string aEntityName, const Tuple &aTuple);

    protected:
        std::string name;

    private:
        void validateCreateTable(SharedEntityPtr& anEntityPtr);
        void validateInsert(SharedEntityPtr& theEntity, const Tuple &aTuple);
        void validateTableExisted(std::string aEntityName);
        void validateTableNotExisted(std::string aEntityName);
    };
}

#endif //DATABASE_H
