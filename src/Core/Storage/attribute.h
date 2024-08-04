//
// Created by chiendd on 21/07/2024.
//

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <string>
#include <vector>

#include "../Util/types.h"

namespace Csql {
    class Attribute {
    public:
        Attribute() {};

        ~Attribute() {};

        Attribute* setName(std::string name);
        Attribute* setType(DataTypes type);
        Attribute* setNullable(bool nullable);
        Attribute* setPrimary(bool primary);
        Attribute* setAutoIncrement(uint32_t autoIncrement);

        std::string getName();
        DataTypes getType();
        bool isNull();
        bool isPrimary();
        bool isAutoIncrement();
        uint32_t getAutoIncrement();


    protected:
        std::string name;
        DataTypes type;
        bool nullable = false;
        bool primary = false;
        uint32_t autoIncrement = 0;
    };

    using AttributeList = std::vector<Attribute*>;
}

#endif //ATTRIBUTE_H
