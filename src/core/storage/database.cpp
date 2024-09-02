//
// Created by chiendd on 21/07/2024.
//

#include "database.h"

#include "entity.h"
#include "../util/errors.h"
#include "../view/query_result_view.h"
#include "../view/table_view.h"

namespace Csql {
    void Database::createTable(std::ostream &anOutput, SharedEntityPtr &anEntity) {
        validateTableNotExisted(anEntity->getName());
        validateCreateTable(anEntity);

        mEntity[anEntity->getName()] = anEntity;

        // make entity file
        std::ofstream f(createEntityPath(anEntity->getName()));
        f.close();

        anEntity->save();
        anEntity->encode(getFstream(anEntity->getName()));

        QueryResultView(0, "create table").show(anOutput);
    }

    void Database::validateCreateTable(SharedEntityPtr &anEntityPtr) {
        int cntPrimaryKey = 0;
        bool valid = true;
        anEntityPtr->eachAttribute([&](Attribute *anAtribute) {
            if (anAtribute->isPrimary()) {
                cntPrimaryKey ++;
            }

            if (anAtribute->getReference()) {
                bool flag = false;

                eachEntity([&](SharedEntityPtr anEntity) {
                    if (anEntity->getAttribute(anAtribute->getReference()->getAttributeName()) &&
                        anEntity->getName() == anAtribute->getReference()->getEntityName()) {
                        flag = true;
                    }
                });

                valid &= flag;
            }
        });

        if (cntPrimaryKey > 1) {
            throw Errors(tooManyPrimaryKeysError);
        }
        if (!valid) {
            throw Errors(tableReferenceNotFoundError);
        }
    }

    void Database::showTables(std::ostream &anOutput) {
        TableView theTableView("Tables in database: " + this->name);
        eachEntity([&](SharedEntityPtr anEntity) {
            theTableView.addData(anEntity->getName());
        });
        theTableView.show(anOutput);
    }

    void Database::describeTable(std::ostream &anOutput, std::string aEntityName) {
        validateTableExisted(aEntityName);

        TableView theTableView("Field", "Type", "Null", "Key", "Default", "Extra");
        getEntity(aEntityName)->eachAttribute([&](Attribute* anAttribute) {
           theTableView.addData(
               anAttribute->getName(),
               "anAttribute->getType()",
               anAttribute->isNull() ? "YES" : "NO",
               anAttribute->isPrimary() ? "PRI" : "",
               "",
               anAttribute->getAutoIncrement() ? "auto_increment" : "");
        });
        theTableView.show(anOutput);
    }

    void Database::dropTable(std::ostream &anOutput, std::string aEntityName) {
        validateTableExisted(aEntityName);

        deleteEntityFile(aEntityName);
        QueryResultView(0, "drop table").show(anOutput);
    }

    void Database::insert(std::ostream &anOutput, std::string aEntityName, const Tuple &aTuple) {
        validateTableExisted(aEntityName);

        SharedEntityPtr theEntity = getEntity(aEntityName);

        validateInsert(theEntity, aTuple);

        bool inserted = false;
        eachDataPage(aEntityName, [&](SharedPagePtr aPage) {
            if (inserted) return;
            if (aPage->addTuple(aTuple)) {
                inserted = true;
                writePage(aPage);
            }
        });

        if (!inserted) {
            SharedPagePtr freePage = popFreePage(aEntityName);
            freePage->addTuple(aTuple);
            pushDataPage(aEntityName, freePage);
            writePage(freePage);
        }

        QueryResultView(1, "insert tuple").show(anOutput);
    }

    void Database::validateInsert(SharedEntityPtr& theEntity, const Tuple &aTuple) {
        theEntity->eachAttribute([&](Attribute* anAttribute) {
            if (!aTuple.contains(anAttribute->getName())) {
                if (!anAttribute->isNull()) {
                    throw Errors(keyNotNullError, anAttribute->getName());
                }
            }
        });
    }

    // make sure the table is existed
    void Database::validateTableExisted(std::string aEntityName) {
        if (!getEntity(aEntityName)) {
            throw Errors(tableNotExistedError);
        }
    }

    // make sure the table isn't existed
    void Database::validateTableNotExisted(std::string aEntityName) {
        if (getEntity(aEntityName)) {
            throw Errors(tableExistedError);
        }
    }
}
