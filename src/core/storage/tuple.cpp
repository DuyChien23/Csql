//
// Created by chiendd on 07/10/2024.
//

#include "tuple.h"
#include "../util/errors.h"

namespace Csql {
    //----------------------------FOR-TUPLE--------------------------------
    SqlTypes Tuple::at(const std::string& attributeName) const {
        for (auto &thePair : data) {
            if (thePair.first == attributeName) {
                return thePair.second;
            }
        }
        throw Errors("Attribute not found in tuple");
    }

    bool Tuple::contains(const std::string &attributeName) const {
        for (auto &thePair : data) {
            if (thePair.first == attributeName) {
                return true;
            }
        }
        return false;
    }


    void Tuple::insert(const std::string& attributeName, SqlTypes value) {
        for (auto &thePair : data) {
            if (thePair.first == attributeName) {
                thePair.second = value;
                return;
            }
        }
        data.push_back({attributeName, value});
    }

    void Tuple::erase(const std::string &attributeName) {
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->first == attributeName) {
                data.erase(it);
                return;
            }
        }
        throw Errors("Attribute not found in tuple");
    }

    //----------------------------FOR-JOINED-TUPLE--------------------------------
    SqlTypes JoinedTuple::at(const std::string &tableName, const std::string &attributeName) const {
        for (auto &thePair : data) {
            if (thePair.first.first == tableName && thePair.first.second == attributeName) {
                return thePair.second;
            }
        }
        throw Errors("Attribute not found in tuple");
    }

    bool JoinedTuple::contains(const std::string &tableName, const std::string &attributeName) const {
        for (auto &thePair : data) {
            if (thePair.first.first == tableName && thePair.first.second == attributeName) {
                return true;
            }
        }
        return false;
    }

    void JoinedTuple::insert(const std::string &tableName, const std::string &attributeName, SqlTypes value) {
        for (auto &thePair : data) {
            if (thePair.first.first == tableName && thePair.first.second == attributeName) {
                thePair.second = value;
                return;
            }
        }
        data.push_back({{tableName, attributeName}, value});
    }

    void JoinedTuple::erase(const std::string &tableName, const std::string &attributeName) {
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->first.first == tableName && it->first.second == attributeName) {
                data.erase(it);
                return;
            }
        }
    }
}
