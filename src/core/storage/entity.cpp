//
// Created by chiendd on 21/07/2024.
//

#include "entity.h"

#include <cassert>

void Entity::save() {
    bufferOffset = 0;

    write(this->name);
    write(this->firstFreePage);
    write(this->nPage);
    write(this->clusteredKey);

    write(this->attributes.size());
    for (auto &attribute: this->attributes) {
        writeAttribute(attribute);
    }

    write(this->indexingMetadata.size());
    for (auto &index: this->indexingMetadata) {
        write(index.root);
        write(index.isClustered);
        write(index.keys);
    }
}

void Entity::refresh() {
    this->bufferOffset = 0;

    read(this->name);
    read(this->firstFreePage);
    read(this->nPage);
    read(this->clusteredKey);

    uint64_t attributeCount;
    read(attributeCount);
    attributes.clear();
    for (int i = 0; i < attributeCount; ++i) {
        attributes.push_back(readAttribute());
    }

    uint64_t indexingMetadataCount;
    read(indexingMetadataCount);
    indexingMetadata.resize(indexingMetadataCount);
    for (auto &index: indexingMetadata) {
        index.entityName = this->name;
        read(index.root);
        read(index.isClustered);
        read(index.keys);
    }
}

Attribute *Entity::readAttribute() {
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
    for (auto &attribute: attributes) {
        theTuple.insert(std::make_pair(attribute->getName(), SqlNullType()));
    }
    return theTuple;
}


void Entity::eachAttribute(const AttributeVisitor &attributeVisitor) {
    for (auto attribute: attributes) {
        attributeVisitor(attribute);
    }
}

void Entity::eachIndexing(const IndexingVisitor &indexingVisitor) {
    assert(indexingMetadata.begin()->isClustered);
    for (auto &index: indexingMetadata) {
        indexingVisitor(index, index.isClustered);
    }
}

//-----------------------------GETER AND SETTER--------------------------------

Attribute *Entity::getAttribute(const std::string &name) {
    for (auto attribute: attributes) {
        if (attribute->getName() == name) {
            return attribute;
        }
    }
    return nullptr;
}

AttributeList &Entity::getAttributes() {
    return attributes;
}

void Entity::addAttribute(Attribute *attribute) {
    if (!getAttribute(attribute->getName())) {
        attributes.push_back(attribute);
    }
}

void Entity::addIndexingMetadata(std::string key, bool isClustered) {
    addIndexingMetadata(std::vector({std::move(key)}), isClustered);
}

void Entity::addIndexingMetadata(std::vector<std::string> keys, bool isClustered) {
    for (auto &index: indexingMetadata) {
        if (index.keys == keys || (index.isClustered && isClustered)) {
            return;
        }
    }
    indexingMetadata.emplace_back(this->name, std::move(keys), isClustered);
}

void Entity::setDatabaseName(std::string dbName) {
    this->dbName = std::move(dbName);
}

void Entity::setFirstFreePage(uint64_t firstFreeBlock) {
    this->firstFreePage = firstFreeBlock;
}

bool Entity::hasFreePage() const {
    return firstFreePage;
}

uint64_t Entity::getFirstFreePage() const {
    return firstFreePage;
}

uint64_t Entity::getIndexNewPage() {
    return nPage++;
}

std::string Entity::getName() {
    return name;
}

void Entity::setClusteredKey(const std::string &name) {
    clusteredKey = name;
}

std::string Entity::getClusteredKey() {
    return clusteredKey;
}
