//
// Created by chiendd on 21/07/2024.
//

#include "entity.h"

#include <unistd.h>

namespace Csql {
    void Entity::save() {
        bufferOffset = 0;

        write(this -> name);
        write(this -> firstDataBlock);
        write(this -> firstFreeBlock);

        write(this -> attributes.size());

        for (auto &attribute : this -> attributes) {
           writeAttribute(attribute);
        }
    }

    void Entity::refresh() {
        this -> bufferOffset = 0;
        uint64_t attributeCount;

        read(this -> name);
        read(this -> firstDataBlock);
        read(this -> firstFreeBlock);
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
        uint32_t autoIncrement;

        read(name);
        read(type);
        read(nullable);
        read(primary);
        read(autoIncrement);

        return (new Attribute())
            ->setName(name)
            ->setType(type)
            ->setNullable(nullable)
            ->setPrimary(primary)
            ->setAutoIncrement(autoIncrement);
    }

    void Entity::writeAttribute(Attribute *anAttribute) {
        write(anAttribute -> getName());
        write(anAttribute -> getType());
        write(anAttribute -> isNull());
        write(anAttribute -> isPrimary());
        write(anAttribute -> getAutoIncrement());
    }


    Attribute* Entity::getAttribute(std::string name) {
        for (auto attribute : attributes) {
            if (attribute -> getName() == name) {
                return attribute;
            }
        }
        return nullptr;
    }

    AttributeList* Entity::getAttributes() {
        return &attributes;
    }


    void Entity::addAttribute(Attribute *attribute) {
        if (!getAttribute(attribute -> getName())) {
            attributes.push_back(attribute);
        }
    }
}
