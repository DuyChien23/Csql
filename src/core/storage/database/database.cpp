//
// Created by chiendd on 21/07/2024.
//

#include "database.h"

#include "../entity.h"
#include "../../util/errors.h"
#include "../../util/helpers.h"
#include "../../view/query_result_view.h"
#include "../../view/table_view.h"
// make sure the table is existed
void Database::validateTableExisted(std::string aEntityName) {
    if (!getEntity(std::move(aEntityName))) {
        throw Errors("Table does not exist");
    }
}

// make sure the table isn't existed
void Database::validateTableNotExisted(std::string aEntityName) {
    if (getEntity(std::move(aEntityName))) {
        throw Errors("Table already exists");
    }
}

std::vector<Tuple> Database::getRowsByCondition(const std::string& entityName, std::vector<Condition> &conditions) {
    SharedEntityPtr theEntity = getEntity(entityName);

    std::vector<Tuple> result;
    theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool isClustered) {
        if (isClustered) {
           auto iter = beginLeaf(indexingMetadata);

            while (iter != endLeaf()) {
                result.push_back((*iter));
                nextLeaf(iter);
            }
        }
    });
    return result;
}

