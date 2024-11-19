//
// Created by chiendd on 21/07/2024.
//

#include "attribute.h"

Reference *Reference::setLimit(uint32_t limit) {
    this->limit = limit;
    return this;
}

Reference *Reference::setEntityName(std::string entityName) {
    this->entityName = entityName;
    return this;
}

Reference *Reference::setAttributeName(std::string attributeName) {
    this->attributeName = attributeName;
    return this;
}

uint32_t Reference::getLimit() {
    return this->limit;
}

std::string Reference::getEntityName() {
    return this->entityName;
}

std::string Reference::getAttributeName() {
    return this->attributeName;
}


Attribute *Attribute::setName(std::string name) {
    this->name = name;
    return this;
}

Attribute *Attribute::setType(DataTypes type) {
    this->type = type;
    return this;
}

Attribute *Attribute::setNullable(bool nullable) {
    this->nullable = nullable;
    return this;
}

Attribute *Attribute::setPrimary(bool primary) {
    this->primary = primary;
    return this;
}

Attribute *Attribute::setUnique(bool unique) {
    this->unique = unique;
    return this;
}

Attribute *Attribute::setAutoIncrement(uint32_t autoIncrement) {
    this->autoIncrement = autoIncrement;
    return this;
}

Attribute *Attribute::setReference(Reference *reference) {
    this->reference = reference;
    return this;
}


std::string Attribute::getName() {
    return this->name;
}

DataTypes Attribute::getType() {
    return this->type;
}

bool Attribute::isNull() {
    if (primary || reference || unique) return false;
    return this->nullable;
}

bool Attribute::isPrimary() {
    return this->primary;
}

bool Attribute::isUnique() {
    if (primary) return true;
    return this->unique;
}


bool Attribute::isAutoIncrement() {
    return this->autoIncrement;
}

uint32_t Attribute::getAutoIncrement() {
    return this->autoIncrement;
}

Reference *Attribute::getReference() {
    return this->reference;
}
