//
// Created by chiendd on 21/07/2024.
//

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <string>
#include <vector>

#include "../util/types.h"

    namespace SpecialKey {
        const std::string BTREE_KEY = "bree_key_2303";
        const std::string HIDDEN_KEY = "hidden_key_2303";
        const std::string CHILD_BTREE_KEY = "bnode_child_2303";
    }

    class Reference {
    public:
        Reference() {};
        ~Reference() {};

        Reference* setLimit(uint32_t limit);
        Reference* setEntityName(std::string entityName);
        Reference* setAttributeName(std::string attributeName);

        uint32_t getLimit();
        std::string getEntityName();
        std::string getAttributeName();
    protected:
        uint32_t limit = 0;
        std::string entityName = "";
        std::string attributeName = "";
    };

    class Attribute {
    public:
        Attribute() {};

        ~Attribute() {};

        Attribute* setName(std::string name);
        Attribute* setType(DataTypes type);
        Attribute* setNullable(bool nullable);
        Attribute* setPrimary(bool primary);
        Attribute* setUnique(bool unique);
        Attribute* setAutoIncrement(uint32_t autoIncrement);
        Attribute* setReference(Reference *reference);

        std::string getName();
        DataTypes getType();
        bool isNull();
        bool isPrimary();
        bool isUnique();
        bool isAutoIncrement();
        uint32_t getAutoIncrement();
        Reference *getReference();

    protected:
        std::string name;
        DataTypes type;
        bool nullable = true;
        bool primary = false;
        bool unique = false;
        uint32_t autoIncrement = 0;
        Reference *reference = nullptr;
    };

    using AttributeList = std::vector<Attribute*>;


#endif //ATTRIBUTE_H
