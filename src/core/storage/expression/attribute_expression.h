//
// Created by chiendd on 10/09/2024.
//

#ifndef COLUMN_EXPRESSION_H
#define COLUMN_EXPRESSION_H
#include "expression.h"
#include "../../util/errors.h"

namespace Csql {
    class AttributeExpression : public Expression {
    public:
        AttributeExpression(std::string _entityName, std::string _attributeName)
            : entityName(std::move(_entityName)), attributeName(std::move(_attributeName)) {};

        SqlTypes apply(const JoinedTuple& tuple) override {
            if (!tuple.contains({entityName, attributeName})) throw Errors("Attribute not found");
            return tuple.at({entityName, attributeName});
        }
    private:
        std::string entityName;
        std::string attributeName;
    };
}

#endif //COLUMN_EXPRESSION_H
