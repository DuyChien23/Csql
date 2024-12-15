//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../view/query_result_view.h"
#include "../../util/errors.h"


void Database::createTable(std::ostream &anOutput, SharedEntityPtr &anEntity) {
    LockManagerInstance::get(name)->lockTable(transactionPtr.get(), LockMode::EXCLUSIVE, anEntity->getName());

    validateTableNotExisted(anEntity->getName());
    validateCreateTable(anEntity);

    mEntity[anEntity->getName()] = anEntity;

    anEntity->addIndexingMetadata(anEntity->getClusteredKey(), true);

    anEntity->eachAttribute([&](Attribute *anAttribute) {
        if ((anAttribute->isPrimary() || anAttribute->isUnique()) && anAttribute->getName() != anEntity->
            getClusteredKey()) {
            anEntity->addIndexingMetadata(anAttribute->getName(), false);
        }
    });

    // make entity file
    std::ofstream f(createEntityPath(anEntity->getName()));
    f.close();

    anEntity->save();
    anEntity->encode(getFstream(anEntity->getName()));

    QueryResultView(0, "create table").show(anOutput);
}

void Database::validateCreateTable(const SharedEntityPtr &anEntityPtr) {
    Attribute *primaryAttribute = nullptr;
    Attribute *uniqueAttribute = nullptr;
    bool valid = true;
    anEntityPtr->eachAttribute([&](Attribute *anAtribute) {
        if (anAtribute->isPrimary()) {
            if (!primaryAttribute) {
                primaryAttribute = anAtribute;
            } else {
                throw Errors("Too many primary keys");
            }
        }

        if (anAtribute->isUnique() && !uniqueAttribute) {
            uniqueAttribute = anAtribute;
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

    if (!valid) {
        throw Errors("Table reference not found");
    }

    if (primaryAttribute) uniqueAttribute = primaryAttribute;

    if (!uniqueAttribute) {
        uniqueAttribute = (new Attribute())->setName(SpecialKey::HIDDEN_KEY)->setType(DataTypes::int_type)->
                setPrimary(true);
        anEntityPtr->addAttribute(uniqueAttribute);
    }
    anEntityPtr->setClusteredKey(uniqueAttribute->getName());
}
