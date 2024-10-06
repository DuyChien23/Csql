//
// Created by chiendd on 07/10/2024.
//

#ifndef TUPLE_H
#define TUPLE_H
#include <string>
#include <vector>

#include "../util/types.h"


namespace Csql {
    class Tuple {
    public:
        Tuple() = default;

        bool contains(const std::string& attributeName) const;
        void insert(const std::string& attributeName, SqlTypes value);
        SqlTypes at(const std::string& attributeName) const;
        void erase(const std::string& attributeName);

        SqlTypes operator[](const std::string& attributeName) const {
            return at(attributeName);
        }

        [[nodiscard]] auto begin() const {
            return data.begin();
        }
        [[nodiscard]] auto end() const {
            return data.end();
        }
    protected:
        std::vector<std::pair<std::string, SqlTypes>> data;
    };

    class JoinedTuple {
    public:
        JoinedTuple() = default;

        bool contains(const std::string& tableName, const std::string& attributeName) const;
        void insert(const std::string &tableName, const std::string &attributeName, SqlTypes value);
        SqlTypes at(const std::string& tableName, const std::string& attributeName) const;
        void erase(const std::string& tableName, const std::string& attributeName);

        SqlTypes operator[](const std::pair<std::string, std::string>& attribute) const {
            return at(attribute.first, attribute.second);
        }
        [[nodiscard]] auto begin() const {
            return data.begin();
        }
        [[nodiscard]] auto end() const {
            return data.end();
        }
    private:
        std::vector<std::pair<std::pair<std::string, std::string>, SqlTypes>> data;
    };
}



#endif //TUPLE_H
