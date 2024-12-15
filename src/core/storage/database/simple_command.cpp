//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../view/query_result_view.h"
#include "../../view/table_view.h"
#include "../../util/helpers.h"

void Database::showTables(std::ostream &anOutput) {
    TableView theTableView("Tables in database: " + this->name);
    eachEntity([&](SharedEntityPtr anEntity) {
        theTableView.addData(anEntity->getName());
    });
    theTableView.show(anOutput);
}

void Database::describeTable(std::ostream &anOutput, std::string aEntityName) {
    validateTableExisted(aEntityName);

    TableView theTableView("Field", "Type", "Null", "Key", "Extra");
    getEntity(aEntityName)->eachAttribute([&](Attribute *anAttribute) {
        theTableView.addData(
            anAttribute->getName(),
            Helpers::SqlTypeHandle::covertDataTypeToString(anAttribute->getType()),
            anAttribute->isNull() ? "YES" : "NO",
            anAttribute->isPrimary() ? "PRI" : anAttribute->isUnique() ? "UNI" : "",
            anAttribute->getAutoIncrement() ? "auto_increment" : "");
    });
    theTableView.show(anOutput);
}

void Database::dropTable(std::ostream &anOutput, std::string aEntityName) {
    validateTableExisted(aEntityName);

    deleteEntityFile(aEntityName);
    QueryResultView(0, "drop table").show(anOutput);
}