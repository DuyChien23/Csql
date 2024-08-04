//
// Created by chiendd on 21/07/2024.
//

#include "attribute.h"

namespace Csql {
    Attribute *Attribute::setName(std::string name) {
        this -> name = name;
        return  this;
    }

    Attribute *Attribute::setType(DataTypes type) {
        this -> type = type;
        return this;
    }

    Attribute *Attribute::setNullable(bool nullable) {
        this -> nullable = nullable;
        return this;
    }

    Attribute *Attribute::setPrimary(bool primary) {
        this -> primary = primary;
        return this;
    }

    Attribute *Attribute::setAutoIncrement(uint32_t autoIncrement) {
        this -> autoIncrement = autoIncrement;
        return this;
    }

    std::string Attribute::getName() {
        return this -> name;
    }

    DataTypes Attribute::getType() {
        return this -> type;
    }

    bool Attribute::isNull() {
        return this -> nullable;
    }

    bool Attribute::isPrimary() {
        return this -> primary;
    }

    bool Attribute::isAutoIncrement() {
        return this -> autoIncrement;
    }

    uint32_t Attribute::getAutoIncrement() {
        return this -> autoIncrement;
    }
}
