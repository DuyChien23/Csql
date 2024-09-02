//
// Created by chiendd on 11/09/2024.
//

#ifndef BETWEEN_EXPRESSION_H
#define BETWEEN_EXPRESSION_H
#include <vector>

#include "binary_expression.h"
#include "expression.h"
#include "../../util/errors.h"

namespace Csql {
    enum class OtherOperator {
        between,
        not_between,
        in,
        not_in,
    };

    class OtherExpression : public Expression {
    public:
        OtherExpression(OtherOperator _op, Expression* _value, std::vector<SqlTypes> _list) {
            op = _op;
            value = _value;
            list = std::move(_list);

            for (int i = 1; i < list.size(); ++i) {
                if (list[i].index() != list[i - 1].index()) {
                    throw Errors(expressionError);
                }
            }

            if ((op == OtherOperator::between || op == OtherOperator::not_between) && list.size() != 2) {
                throw Errors(expressionError);
            }
        };

        SqlTypes apply(const JoinedTuple &tuple) override {
            SqlTypes v = value->apply(tuple);
            if (!list.empty() && v.index() != list.begin()->index()) throw Errors(expressionError);

            if (op == OtherOperator::between || op == OtherOperator::not_between) {
                return static_cast<SqlBoolType>((op == OtherOperator::not_between) ^
                    (Helpers::compareSqlType(list[0], v) != SqlTypeCompareResult::greater
                        && Helpers::compareSqlType(v, list[1]) != SqlTypeCompareResult::greater));
            } else {
                bool is_in = false;
                for (auto& element : list) {
                    if (Helpers::compareSqlType(v, element) == SqlTypeCompareResult::equal) {
                        is_in = true;
                    }
                }
                return static_cast<SqlBoolType>((op == OtherOperator::not_in) ^ is_in);
            }
        }
    private:
        OtherOperator op;
        Expression* value;
        std::vector<SqlTypes> list;
    };
}

#endif //BETWEEN_EXPRESSION_H
