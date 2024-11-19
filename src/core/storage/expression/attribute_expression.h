//
// Created by chiendd on 10/09/2024.
//

#ifndef COLUMN_EXPRESSION_H
#define COLUMN_EXPRESSION_H
#include "expression.h"
#include "../../util/errors.h"

class AttributeExpression : public Expression {
public:
    AttributeExpression(std::string _entityName, std::string _attributeName)
        : entityName(std::move(_entityName)), attributeName(std::move(_attributeName)) {
    };

    SqlTypes apply(const JoinedTuple &tuple) override {
        if (attributeName.empty()) {
            int count = 0;
            SqlTypes result;
            for (auto &pair: tuple) {
                if (pair.first.second == entityName) {
                    count++;
                    result = pair.second;
                }
            }

            if (count == 0) throw Errors("Attribute not found");
            if (count > 1) throw Errors("Ambiguous attribute");
            return result;
        }
        if (!tuple.contains({entityName, attributeName})) throw Errors("Attribute not found");
        return tuple.at({entityName, attributeName});
    }

private:
    std::string entityName;
    std::string attributeName;
};


#endif //COLUMN_EXPRESSION_H
