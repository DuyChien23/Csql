//
// Created by chiendd on 08/10/2024.
//
#include "expression_parser.h"

#include <stack>

#include "../../storage/expression/arithmetic_expresstion.h"
#include "../../storage/expression/attribute_expression.h"
#include "../../storage/expression/binary_expression.h"
#include "../../storage/expression/function_expression.h"
#include "../../storage/expression/other_expression.h"
#include "../../storage/expression/set_expression.h"
#include "../../storage/expression/unary_expression.h"
#include "../../storage/expression/value_expression.h"
#include "../../util/helpers.h"

SqlTypes covertTokenToSqlType(const Token *token);

int getPriority(SqlOperators op);

std::map<SqlOperators, UnaryOperator> ExpressionParser::unaryMapping = {
    {SqlOperators::not_op, UnaryOperator::is_not}
};

std::map<SqlOperators, BinaryOperator> ExpressionParser::binaryMapping = {
    {SqlOperators::equal_op, BinaryOperator::equal},
    {SqlOperators::not_equal_op, BinaryOperator::not_equal},
    {SqlOperators::gt_op, BinaryOperator::greater},
    {SqlOperators::gte_op, BinaryOperator::greater_or_equal},
    {SqlOperators::lt_op, BinaryOperator::less_than},
    {SqlOperators::lte_op, BinaryOperator::less_than_or_equal},
    {SqlOperators::and_op, BinaryOperator::logic_and},
    {SqlOperators::or_op, BinaryOperator::logic_or},
    {SqlOperators::like_op, BinaryOperator::like},
    {SqlOperators::not_like_op, BinaryOperator::not_like},
};

std::map<SqlOperators, ArithmeticOperator> ExpressionParser::arithmeticMapping = {
    {SqlOperators::add_op, ArithmeticOperator::addition},
    {SqlOperators::subtract_op, ArithmeticOperator::subtraction},
    {SqlOperators::multiply_op, ArithmeticOperator::multiplication},
    {SqlOperators::divide_op, ArithmeticOperator::division},
    {SqlOperators::mod_op, ArithmeticOperator::modulus},
};

std::map<SqlOperators, OtherOperator> ExpressionParser::otherMapping = {
    {SqlOperators::between_op, OtherOperator::between},
    {SqlOperators::in_op, OtherOperator::in},
    {SqlOperators::not_between_op, OtherOperator::not_between},
    {SqlOperators::not_in_op, OtherOperator::not_in},
};

std::map<SqlFunctions, FunctionTypes> ExpressionParser::functionMapping = {
    {SqlFunctions::ascii_ft, FunctionTypes::ascii_ft},
    {SqlFunctions::concat_ft, FunctionTypes::concat_ft},
    {SqlFunctions::concat_ws_ft, FunctionTypes::concat_ws_ft},
    {SqlFunctions::char_length_ft, FunctionTypes::char_length_ft},
    {SqlFunctions::char_ft, FunctionTypes::char_ft},
    {SqlFunctions::length_ft, FunctionTypes::length_ft},
    {SqlFunctions::left_ft, FunctionTypes::left_ft},
    {SqlFunctions::locate_ft, FunctionTypes::locate_ft},
    {SqlFunctions::lower_ft, FunctionTypes::lower_ft},
    {SqlFunctions::substring_ft, FunctionTypes::substring_ft},
    {SqlFunctions::replace_ft, FunctionTypes::replace_ft},
    {SqlFunctions::right_ft, FunctionTypes::right_ft},
    {SqlFunctions::reverse_ft, FunctionTypes::reverse_ft},
    {SqlFunctions::trim_ft, FunctionTypes::trim_ft},
    {SqlFunctions::upper_ft, FunctionTypes::upper_ft},
};

Expression *ExpressionParser::parse(std::string endToken) {
    std::stack<Expression *> result;
    std::stack<SqlOperators> ops;

    auto exeOp = [&](SqlOperators op) {
        if (unaryMapping.contains(op)) {
            auto v = result.top(); result.pop();
            result.push(new UnaryExpression(unaryMapping[op], v));
            return;
        }
        auto r = result.top(); result.pop();
        auto l = result.top(); result.pop();
        if (binaryMapping.contains(op)) {
            result.push(new BinaryExpression(binaryMapping[op], l, r));
        }
        if (arithmeticMapping.contains(op)) {
            result.push(new ArithmeticExpression(arithmeticMapping[op], l, r));
        }
        if (otherMapping.contains(op)) {
            result.push(new OtherExpression(otherMapping[op], l, r));
        }
    };

    while (isContinued(endToken)) {
        std::string tableName, attributeName;
        if (tokenizer->consumeAttribute(tableName, attributeName)) {
            result.push(new AttributeExpression(tableName, attributeName));
            continue;
        }

        if (tokenizer->currentIs(TokenType::function)) {
            SqlFunctions ft;
            tokenizer->consumeType(ft);
            tokenizer->check("(");
            std::vector<Expression *> parameters(0);
            while (!tokenizer->currentIsChar(')')) {
                parameters.push_back(parse(")"));
                if (tokenizer->currentIsChar(',')) {
                    tokenizer->checkOne();
                }
            }
            tokenizer->check(")");

            result.push(new FunctionExpression(functionMapping[ft], parameters));
            continue;
        }

        if (tokenizer->currentIsChar('(')) {
            tokenizer->checkOne();
            ops.push(SqlOperators::open_parenthesis);
            continue;
        }

        if (tokenizer->currentIsChar(')')) {
            tokenizer->checkOne();
            while (!ops.empty() && ops.top() != SqlOperators::open_parenthesis) {
                exeOp(ops.top());
                ops.pop();
            }
            if (ops.empty()) {
                throw Errors("Invalid expression");
            }
            ops.pop();
            continue;
        }

        if (tokenizer->currentIs(SqlOperators::not_op, SqlOperators::between_op, SqlOperators::in_op,
                                 SqlOperators::like_op)) {
            bool isNot = false;

            if (tokenizer->currentIs(SqlOperators::not_op)) {
                isNot = true;
                tokenizer->checkOne();
            }

            if (isNot && !tokenizer->currentIs(SqlOperators::between_op, SqlOperators::in_op, SqlOperators::like_op)) {
                ops.push(SqlOperators::not_op);
                continue;
            }

            if (tokenizer->currentIs(SqlOperators::between_op)) {
                tokenizer->checkOne();
                std::vector<SqlTypes> ranges(0);
                ranges.push_back(covertTokenToSqlType(tokenizer->get()));
                tokenizer->check(SqlOperators::add_op);
                ranges.push_back(covertTokenToSqlType(tokenizer->get()));

                ops.push(isNot ? SqlOperators::not_between_op : SqlOperators::between_op);
                result.push(new SetExpression(SetTypes::range, ranges));
            } else if (tokenizer->currentIs(SqlOperators::in_op)) {
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

                ops.push(isNot ? SqlOperators::not_in_op : SqlOperators::in_op);
                result.push(new SetExpression(SetTypes::list, theSet));
            } else {
                tokenizer->checkOne();
                ops.push(isNot ? SqlOperators::not_like_op : SqlOperators::like_op);
            }

            continue;
        }

        if (tokenizer->currentIs(TokenType::operators)) {
            while (!ops.empty() && getPriority(ops.top()) >= getPriority(tokenizer->peek()->op)) {
                exeOp(ops.top());
                ops.pop();
            }
            ops.push(tokenizer->peek()->op);
            tokenizer->checkOne();
            continue;
        }

        result.push(new ValueExpression(covertTokenToSqlType(tokenizer->get())));
    }

    while (!ops.empty()) {
        exeOp(ops.top());
        ops.pop();
    }

    if (result.size() != 1) {
        throw Errors("Invalid expression");
    }
    return result.top();
}

SqlTypes covertTokenToSqlType(const Token *token) {
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

int getPriority(SqlOperators op) {
    if (op == SqlOperators::multiply_op || op == SqlOperators::divide_op || op == SqlOperators::mod_op) {
        return 5;
    }
    if (op == SqlOperators::add_op || op == SqlOperators::subtract_op) {
        return 4;
    }
    if (op == SqlOperators::equal_op || op == SqlOperators::not_equal_op || op == SqlOperators::gt_op ||
        op == SqlOperators::gte_op || op == SqlOperators::lt_op || op == SqlOperators::lte_op) {
        return 3;
    }
    if (op == SqlOperators::not_op || op == SqlOperators::like_op || op == SqlOperators::in_op || op ==
        SqlOperators::between_op || op == SqlOperators::not_like_op || op == SqlOperators::not_in_op || op ==
        SqlOperators::not_between_op) {
        return 2;
    }
    if (op == SqlOperators::and_op || op == SqlOperators::or_op) {
        return 1;
    }

    return 0;
}
