//
// Created by chiendd on 21/07/2024.
//

#include "entity.h"

namespace Csql {
    void Entity::save() {
        bufferOffset = 0;

        write(this->name);
        write(this->firstDataPage);
        write(this->firstFreePage);
        write(this->nPage);

        write(this->attributes.size());

        for (auto &attribute : this->attributes) {
           writeAttribute(attribute);
        }
    }

    void Entity::refresh() {
        this->bufferOffset = 0;
        uint64_t attributeCount;

        read(this->name);
        read(this->firstDataPage);
        read(this->firstFreePage);
        read(this->nPage);
        read(attributeCount);
        attributes.clear();

        for (int i = 0; i < attributeCount; ++i) {
            attributes.push_back(readAttribute());
        }
    }

    Attribute* Entity::readAttribute() {
        std::string name;
        DataTypes type;
        bool nullable;
        bool primary;
        bool unique;
        uint32_t autoIncrement;

        read(name);
        read(type);
        read(nullable);
        read(primary);
        read(unique);
        read(autoIncrement);
        Reference *reference = readReference();

        return (new Attribute())
            ->setName(name)
            ->setType(type)
            ->setNullable(nullable)
            ->setPrimary(primary)
            ->setUnique(unique)
            ->setAutoIncrement(autoIncrement)
            ->setReference(reference);
    }

    void Entity::writeAttribute(Attribute *anAttribute) {
        write(anAttribute->getName());
        write(anAttribute->getType());
        write(anAttribute->isNull());
        write(anAttribute->isPrimary());
        write(anAttribute->isUnique());
        write(anAttribute->getAutoIncrement());
        writeReference(anAttribute->getReference());
    }

    Reference *Entity::readReference() {
        bool referred;
        read(referred);

        if (!referred) {
            return nullptr;
        }

        uint32_t limit;
        std::string entityName;
        std::string attributeName;

        read(limit);
        read(entityName);
        read(attributeName);

        return (new Reference)
            ->setLimit(limit)
            ->setEntityName(entityName)
            ->setAttributeName(attributeName);
    }

    void Entity::writeReference(Reference *aReference) {
        if (aReference) {
            write(true);
            write(aReference->getLimit());
            write(aReference->getEntityName());
            write(aReference->getAttributeName());
        } else {
            write(false);
        }
    }

    Tuple Entity::baseNullTuple() {
        Tuple theTuple;
        for (auto &attribute : attributes) {
            theTuple.insert(attribute->getName(), SqlNullType());
        }
        return theTuple;
    }


    void Entity::eachAttribute(const AttributeVisitor &attributeVisitor) {
        for (auto attribute : attributes) {
            attributeVisitor(attribute);
        }
    }

    //-----------------------------GETER AND SETTER--------------------------------

    Attribute* Entity::getAttribute(const std::string& name) {
        for (auto attribute : attributes) {
            if (attribute->getName() == name) {
                return attribute;
            }
        }
        return nullptr;
    }

    AttributeList* Entity::getAttributes() {
        return &attributes;
    }

    void Entity::addAttribute(Attribute *attribute) {
        if (!getAttribute(attribute->getName())) {
            attributes.push_back(attribute);
        }
    }

    void Entity::setDatabaseName(std::string dbName) {
        this->dbName = dbName;
    }

    void Entity::setFirstDataPage(uint64_t firstDataBlock) {
        this->firstDataPage = firstDataBlock;
    }

    uint64_t Entity::getFirstDataPage() {
        return firstDataPage;
    }

    void Entity::setFirstFreePage(uint64_t firstFreeBlock) {
        this->firstFreePage = firstFreeBlock;
    }

    bool Entity::hasDataPage() {
        return firstDataPage;
    }

    bool Entity::hasFreePage() {
        return firstFreePage;
    }

    uint64_t Entity::getFirstFreePage() {
        return firstFreePage;
    }

    uint64_t Entity::getIndexNewPage() {
        return nPage++;
    }

    std::string Entity::getName() {
        return name;
    }
}
