//
// Created by chiendd on 08/10/2024.
//
#include "expression_parser.h"

#include "../../storage/expression/arithmetic_expresstion.h"
#include "../../storage/expression/attribute_expression.h"
#include "../../storage/expression/binary_expression.h"
#include "../../storage/expression/other_expression.h"
#include "../../storage/expression/value_expression.h"
#include "../../util/helpers.h"


namespace Csql {
    SqlTypes covertTokenToSqlType(const Token* token);
    BinaryOperator covertSqlOperatorToBinaryOperator(SqlOperators op);
    ArithmeticOperator covertSqlOperatorToArithmeticOperator(SqlOperators op);

    Expression *ExpressionParser::parse() {
        return parseExpression();
    }

    Expression *ExpressionParser::parseExpression(bool isLogicExpression) {
        Expression *result = parseTerm(isLogicExpression);
        if (isLogicExpression) {
            while (isContinued()) {
                if (tokenizer->currentIs(SqlKeywords::and_kw)) {
                    tokenizer->checkOne();
                    result = new BinaryExpression(BinaryOperator::logic_and, result, parseTerm(isLogicExpression));
                } else if (tokenizer->currentIs(SqlKeywords::or_kw)) {
                    tokenizer->checkOne();
                    result = new BinaryExpression(BinaryOperator::logic_or, result, parseTerm(isLogicExpression));
                } else {
                    break;
                }
            }
        } else {
            while (isContinued()) {
                if (tokenizer->currentIs(SqlOperators::add_op, SqlOperators::subtract_op)) {
                    result = new ArithmeticExpression(
                        covertSqlOperatorToArithmeticOperator(tokenizer->get()->op),
                        result,
                        parseTerm(isLogicExpression));
                } else {
                    break;
                }
            }
        }

        return result;
    }

    Expression *ExpressionParser::parseTerm(bool isLogicExpression) {
        if (isLogicExpression) {
            Expression *result = parseFactor(isLogicExpression);
            bool isNot = false;
            if (tokenizer->currentIs(SqlKeywords::not_kw)) {
                tokenizer->checkOne();
                isNot = true;
            }

            if (tokenizer->currentIs(SqlKeywords::between_kw)) {
                tokenizer->checkOne();
                std::vector<SqlTypes> ranges(0);
                ranges.push_back(covertTokenToSqlType(tokenizer->get()));
                tokenizer->check(SqlKeywords::and_kw);
                ranges.push_back(covertTokenToSqlType(tokenizer->get()));
                return new OtherExpression(isNot ? OtherOperator::not_between : OtherOperator::between, result, ranges);
            } else if (tokenizer->currentIs(SqlKeywords::in_kw)) {
                tokenizer->checkOne();
                std::vector<SqlTypes> theSet(0);
                tokenizer->check("[");
                while (!tokenizer->currentIsChar(']')) {
                    theSet.push_back(covertTokenToSqlType(tokenizer->get()));
                    if (tokenizer->currentIsChar(',')) {
                        tokenizer->checkOne();
                    }
                }
                tokenizer->check("]");
                return new OtherExpression(isNot ? OtherOperator::not_in : OtherOperator::in, result, theSet);
            } else if (tokenizer->currentIs(SqlOperators::equal_op, SqlOperators::gt_op, SqlOperators::gte_op,
                                            SqlOperators::lt_op, SqlOperators::lte_op, SqlOperators::not_equal_op)) {
                SqlOperators op;
                tokenizer->consumeType(op);
                Expression* right = parseFactor(isLogicExpression);
                return new BinaryExpression(covertSqlOperatorToBinaryOperator(op), result, right);
            }
            return result;
        } else {
            Expression *result;
            while (isContinued()) {
                if (tokenizer->currentIs(SqlOperators::multiply_op, SqlOperators::divide_op, SqlOperators::mod_op)) {
                    result = new ArithmeticExpression(
                        covertSqlOperatorToArithmeticOperator(tokenizer->get()->op),
                        result,
                        parseTerm(isLogicExpression));
                } else {
                    break;
                }
            }
            return result;
        }
    }

    Expression *ExpressionParser::parseFactor(bool isLogicExpression) {
        if (!isContinued()) return nullptr;

        if (tokenizer->currentIsChar('(')) {
            tokenizer->checkOne();
            Expression *expression = parseExpression(isLogicExpression);
            tokenizer->check(")");
            return expression;
        }

        std::string tableName, attributeName;
        if (tokenizer->consumeAttribute(tableName, attributeName)) {
            return new AttributeExpression(tableName, attributeName);
        }

        return parseValue();
    }

    Expression *ExpressionParser::parseValue() {
        if (!isContinued()) return nullptr;
        return new ValueExpression(covertTokenToSqlType(tokenizer->get()));
    }

    SqlTypes covertTokenToSqlType(const Token* token) {
        if (token->type == TokenType::number) {
            if (token->data.find('.') != std::string::npos) {
                return Helpers::SqlTypeHandle::covertStringToSqlType(DataTypes::float_type, token->data);
            } else {
                return Helpers::SqlTypeHandle::covertStringToSqlType(DataTypes::int_type, token->data);
            }
        }
        if (token->type == TokenType::keyword) {
            if (token->keyword == SqlKeywords::true_kw || token->keyword == SqlKeywords::false_kw) {
                return Helpers::SqlTypeHandle::covertStringToSqlType(DataTypes::bool_type, token->data);
            }
        }
        if (token->type == TokenType::string) {
            return Helpers::SqlTypeHandle::covertStringToSqlType(DataTypes::varchar_type, token->data);
        }
        throw Errors("Unexpected token: " + token->data);
    }

    BinaryOperator covertSqlOperatorToBinaryOperator(SqlOperators op) {
        switch (op) {
            case SqlOperators::equal_op:
                return BinaryOperator::equal;
            case SqlOperators::gt_op:
                return BinaryOperator::greater;
            case SqlOperators::gte_op:
                return BinaryOperator::greater_or_equal;
            case SqlOperators::lt_op:
                return BinaryOperator::less_than;
            case SqlOperators::lte_op:
                return BinaryOperator::less_than_or_equal;
            case SqlOperators::not_equal_op:
                return BinaryOperator::not_equal;
            default:
                throw Errors("Unexpected operator: " + std::to_string(static_cast<int>(op)));
        }
    }

    ArithmeticOperator covertSqlOperatorToArithmeticOperator(SqlOperators op) {
        switch (op) {
            case SqlOperators::add_op:
                return ArithmeticOperator::addition;
            case SqlOperators::subtract_op:
                return ArithmeticOperator::subtraction;
            case SqlOperators::multiply_op:
                return ArithmeticOperator::multiplication;
            case SqlOperators::divide_op:
                return ArithmeticOperator::division;
            case SqlOperators::mod_op:
                return ArithmeticOperator::modulus;
            default:
                throw Errors("Unexpected operator: " + std::to_string(static_cast<int>(op)));
        }
    }
}
