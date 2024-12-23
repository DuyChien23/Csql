//
// Created by chiendd on 26/09/2024.
//

#include "sql_parser.h"

#include "../../storage/entity.h"
#include "../../storage/expression/value_expression.h"
#include "../../storage/expression/aggregate_expression/avg_expression.h"
#include "../../storage/expression/aggregate_expression/count_all_expression.h"
#include "../../storage/expression/aggregate_expression/count_expression.h"
#include "../../storage/expression/aggregate_expression/group_concat_expression.h"
#include "../../storage/expression/aggregate_expression/max_expression.h"
#include "../../storage/expression/aggregate_expression/min_expression.h"
#include "../../storage/expression/aggregate_expression/normal_expression.h"
#include "../../storage/expression/aggregate_expression/sum_expression.h"

#include "../statements/table_statements/create_table_statement.h"
#include "../statements/table_statements/delete_tuples_statement.h"
#include "../statements/table_statements/describe_table_statement.h"
#include "../statements/table_statements/drop_table_statement.h"
#include "../statements/table_statements/insert_statement.h"
#include "../statements/table_statements/select_statement.h"
#include "../statements/table_statements/update_statement.h"
#include "../tokenizer/expression_parser.h"

void getListString(Tokenizer *aTokenizer, std::vector<std::string> &list);

bool makeJoinExpression(Tokenizer *aTokenizer, ExpressionParser &expressionParser, SQLQueryPtr &theQuery);

void makeWhereExpression(Tokenizer *aTokenizer, ExpressionParser &expressionParser, SQLQueryPtr &theQuery);

//-------------------------------------------------------------------------------------------------
Statement *SqlParser::makeStatement(Tokenizer &aTokenizer) {
    for (auto &factory: factories) {
        try {
            Statement *statement = (this->*factory)(aTokenizer.reset());
            if (statement != nullptr) {
                return statement;
            }
        } catch (...) {
            // do nothing because we want to try the next factory
        }
    }
    return nullptr;
}

Statement *SqlParser::createEntityStatement(Tokenizer *aTokenizer) {
    std::string theEntityName;
    aTokenizer->check(SqlKeywords::create_kw)->check(SqlKeywords::table_kw)->consumeType(theEntityName)->check("(");

    SharedEntityPtr theEntity = std::make_shared<Entity>(theEntityName);
    std::vector<std::pair<std::string, std::pair<std::string, std::string> > > foreignKeys;

    while (!aTokenizer->currentIsChar(')')) {
        if (aTokenizer->currentIs(SqlKeywords::foreign_kw)) {
            std::string attributeName;
            aTokenizer->check(SqlKeywords::foreign_kw)->check(SqlKeywords::key_kw)->check("(")->
                    consumeType(attributeName)->check(")");
            std::string refEntityName, refAttributeName;
            aTokenizer->check(SqlKeywords::references_kw)->consumeType(refEntityName)->check("(")->
                    consumeType(refAttributeName)->check(")");
            foreignKeys.push_back({attributeName, {refEntityName, refAttributeName}});
        } else {
            Attribute *theAtrribute = new Attribute();
            std::string attributeName, type;
            aTokenizer->consumeType(attributeName)->consumeType(type);
            theAtrribute->setName(attributeName);
            theAtrribute->setType(Helpers::SqlTypeHandle::covertStringToDataType(type));

            if (theAtrribute->getType() == DataTypes::varchar_type) {
                int size;
                aTokenizer->check("(")->consumeType(size)->check(")");
                // TODO: set size
            }
            if (theAtrribute->getType() == DataTypes::null_type) {
                delete theAtrribute;
                return nullptr;
            }

            while (!aTokenizer->currentIsChar(',')) {
                if (aTokenizer->currentIsChar(')')) {
                    break;
                }

                if (aTokenizer->currentIs(SqlOperators::not_op)) {
                    aTokenizer->checkOne();
                    aTokenizer->check(SqlKeywords::null_kw);
                    theAtrribute->setNullable(false);
                } else {
                    SqlKeywords theKeyword;
                    aTokenizer->consumeType(theKeyword);
                    switch (theKeyword) {
                        case SqlKeywords::primary_kw:
                            aTokenizer->check(SqlKeywords::key_kw);
                        theAtrribute->setPrimary(true);
                        break;
                        case SqlKeywords::auto_increment:
                            theAtrribute->setAutoIncrement(true);
                        break;
                        case SqlKeywords::unique_kw:
                            theAtrribute->setUnique(true);
                        break;
                        default:
                            return nullptr;
                    }
                }
            }

            theEntity->addAttribute(theAtrribute);
        }
        if (aTokenizer->currentIsChar(')')) {
            break;
        }
        aTokenizer->check(",");
    }
    aTokenizer->check(")");
    aTokenizer->endBy(";");

    for (auto foreignKey: foreignKeys) {
        auto theAttribute = theEntity->getAttribute(foreignKey.first);
        if (!theAttribute) throw Errors("Reference to non-existing attribute");

        theEntity->getAttribute(foreignKey.first)->setReference(
            (new Reference)->setEntityName(foreignKey.second.first)->setAttributeName(foreignKey.second.second)
        );
    }

    return new CreateTableStatement(theEntity, output);
}

Statement *SqlParser::describeEntityStatement(Tokenizer *aTokenizer) {
    std::string theEntityName;
    aTokenizer->check(SqlKeywords::describe_kw)->check(SqlKeywords::table_kw)->consumeType(theEntityName)->endBy(";");

    return new DescribeTableStatement(theEntityName, output);
}

Statement *SqlParser::dropEntityStatement(Tokenizer *aTokenizer) {
    std::string theEntityName;
    aTokenizer->check(SqlKeywords::drop_kw)->check(SqlKeywords::table_kw)->consumeType(theEntityName)->endBy(";");

    return new DropTableStatement(theEntityName, output);
}

Statement *SqlParser::insertStatement(Tokenizer *aTokenizer) {
    std::string theEntityName;
    aTokenizer->check(SqlKeywords::insert_kw)->check(SqlKeywords::into_kw)->consumeType(theEntityName);

    std::vector<std::string> attributeNames;
    std::vector<std::string> values;

    if (aTokenizer->currentIsChar('(')) {
        getListString(aTokenizer, attributeNames);
    }

    aTokenizer->check(SqlKeywords::values_kw);
    getListString(aTokenizer, values);
    aTokenizer->endBy(";");

    return new InsertStatement(theEntityName, output, attributeNames, values);
}

// get list string from tokenizer. format: (string1, string2, string3, ...)
void getListString(Tokenizer *aTokenizer, std::vector<std::string> &list) {
    aTokenizer->check("(");
    while (!aTokenizer->currentIsChar(')')) {
        std::string attributeName;
        aTokenizer->consumeType(attributeName);

        list.push_back(attributeName);
        if (aTokenizer->currentIsChar(')')) {
            break;
        }
        aTokenizer->check(",");
    }
    aTokenizer->check(")");
}

Statement *SqlParser::selectStatement(Tokenizer *aTokenizer) {
    auto terminateCheck = [&]() -> bool {
        return aTokenizer->currentIs(TokenType::keyword) || aTokenizer->currentIsChar(';');
    };

    //================================SELECT===============================
    aTokenizer->check(SqlKeywords::select_kw);
    SQLQueryPtr theQuery = std::make_unique<SQLQuery>();

    if (aTokenizer->peek()->data == "*") {
        aTokenizer->checkOne();
    } else {
        while (!terminateCheck()) {
            std::string s1, s2, columnName;
            TargetExpression* target;

            if (aTokenizer->consumeAttribute(s1, s2)) {
                target = new NormalExpression(s1, s2);
                columnName = (s1.empty() ? s2 : s1 + "." + s2);
            } else {
                columnName = aTokenizer->getStringUntil(")");
                SqlFunctions theFunction;
                aTokenizer->consumeType(theFunction);

                aTokenizer->check("(");

                if (theFunction == SqlFunctions::count_ft && aTokenizer->peek()->data == "*") {
                    aTokenizer->checkOne();
                    target = new CountAllExpression();
                } else {
                    ExpressionParser expressionParser(aTokenizer);
                    Expression* expression = expressionParser.parse(")");

                    switch (theFunction) {
                        case SqlFunctions::count_ft: {
                            target = new CountExpression(expression);
                            break;
                        }
                        case SqlFunctions::sum_ft: {
                            target = new SumExpression(expression);
                            break;
                        }
                        case SqlFunctions::avg_ft: {
                            target = new AvgExpression(expression);
                            break;
                        }
                        case SqlFunctions::max_ft: {
                            target = new MaxExpression(expression);
                            break;
                        }
                        case SqlFunctions::min_ft: {
                            target = new MinExpression(expression);
                            break;
                        }
                        case SqlFunctions::group_concat_ft: {
                            std::string split = ",";
                            if (aTokenizer->currentIsChar(',')) {
                                aTokenizer->checkOne();
                                aTokenizer->consumeType(split);
                            }
                            target = new GroupConcatExpression(expression, split);
                            break;
                        }
                        default:
                            throw Errors("Function not found");
                    }
                }

                aTokenizer->check(")");
            }


            if (aTokenizer->currentIs(SqlKeywords::as_kw)) {
                aTokenizer->check(SqlKeywords::as_kw)->consumeType(columnName);
            }

            theQuery->addTarget(target, columnName);

            if (terminateCheck()) {
                break;
            }
            aTokenizer->check(",");
        }
    }

    //================================FROM================================
    std::string entityName;
    aTokenizer->check(SqlKeywords::from_kw)->consumeType(entityName);

    theQuery->setEntityName(entityName);

    //================================JOIN================================
    ExpressionParser expressionParser(aTokenizer);
    while (makeJoinExpression(aTokenizer, expressionParser, theQuery));

    //=================================WHERE===============================
    makeWhereExpression(aTokenizer, expressionParser, theQuery);
    //=================================GROUP BY============================
    if (aTokenizer->currentIs(SqlKeywords::group_kw)) {
        aTokenizer->check(SqlKeywords::group_kw)->check(SqlKeywords::by_kw);
        while (!terminateCheck()) {
            std::string tableName, attributeName;
            aTokenizer->consumeAttribute(tableName, attributeName);
            theQuery->addGroupCondition(tableName, attributeName);
            if (terminateCheck()) {
                break;
            }
            aTokenizer->check(",");
        }
    }
    //=================================ORDER BY============================
    if (aTokenizer->currentIs(SqlKeywords::order_kw)) {
        aTokenizer->check(SqlKeywords::order_kw)->check(SqlKeywords::by_kw);

        std::string tableName, attributeName;
        while (aTokenizer->consumeAttribute(tableName, attributeName)) {
            OrderType orderType = OrderType::asc;
            if (aTokenizer->currentIs(SqlKeywords::desc_kw)) {
                aTokenizer->check(SqlKeywords::desc_kw);
                orderType = OrderType::desc;
            } else {
                if (aTokenizer->currentIs(SqlKeywords::asc_kw)) {
                    aTokenizer->checkOne();
                }
            }
            theQuery->addOrderCondition(tableName, attributeName, orderType);

            if (aTokenizer->currentIsChar(',')) {
                aTokenizer->check(",");
            }
        }
    }
    //==================================LIMIT================================
    if (aTokenizer->currentIs(SqlKeywords::limit_kw)) {
        int limit, offset = 0;
        aTokenizer->check(SqlKeywords::limit_kw)->consumeType(limit);
        if (aTokenizer->currentIs(SqlKeywords::offset_kw)) {
            aTokenizer->check(SqlKeywords::offset_kw)->consumeType(offset);
        }
        theQuery->setLimit(limit, offset);
    }
    //==================================END==================================
    aTokenizer->endBy(";");

    return new SelectStatement(theQuery, output);
}

bool makeJoinExpression(Tokenizer *aTokenizer, ExpressionParser &expressionParser, SQLQueryPtr &theQuery) {
    if (aTokenizer->currentIs(SqlKeywords::inner_kw) || aTokenizer->currentIs(SqlKeywords::left_kw) || aTokenizer->
        currentIs(SqlKeywords::right_kw) || aTokenizer->currentIs(SqlKeywords::full_kw) || aTokenizer->currentIs(
            SqlKeywords::cross_kw)) {
        SqlKeywords joinTypeKW;
        std::string entityName;

        aTokenizer->consumeType(joinTypeKW)->check(SqlKeywords::join_kw)->consumeType(entityName);

        JoinTypes joinType;
        switch (joinTypeKW) {
            case SqlKeywords::inner_kw:
                joinType = JoinTypes::inner;
                break;
            case SqlKeywords::left_kw:
                joinType = JoinTypes::left;
                break;
            case SqlKeywords::right_kw:
                joinType = JoinTypes::right;
                break;
            case SqlKeywords::full_kw:
                joinType = JoinTypes::full;
                break;
            case SqlKeywords::cross_kw:
                joinType = JoinTypes::cross;
                break;
            default:
                throw Errors("Join type not found");
        }

        if (joinType == JoinTypes::cross) {
            theQuery->addJoinExpression(JoinExpression(new ValueExpression(true), joinType, entityName));
            return true;
        }

        aTokenizer->check(SqlKeywords::on_kw);

        theQuery->addJoinExpression(JoinExpression(expressionParser.parse(), joinType, entityName));
        return true;
    }
    return false;
}

Statement *SqlParser::deleteTuplesStatement(Tokenizer *aTokenizer) {
    SQLQueryPtr theQuery = std::make_unique<SQLQuery>();

    std::string entityName;
    aTokenizer->check(SqlKeywords::delete_kw)->check(SqlKeywords::from_kw)->consumeType(entityName);
    theQuery->setEntityName(entityName);

    ExpressionParser expressionParser(aTokenizer);
    makeWhereExpression(aTokenizer, expressionParser, theQuery);

    aTokenizer->endBy(";");

    return new DeleteTuplesStatement(theQuery, output);
}

Statement *SqlParser::updateStatement(Tokenizer *aTokenizer) {
    UpdateQueryPtr theQuery = std::make_unique<UpdateQuery>();

    std::string entityName;
    aTokenizer->check(SqlKeywords::update_kw)->consumeType(entityName);
    theQuery->setEntityName(entityName);
    aTokenizer->check(SqlKeywords::set_kw);

    while (true) {
        std::string attributeName;
        aTokenizer->consumeType(attributeName)->check("=");

        ExpressionParser expressionParser(aTokenizer);
        theQuery->addUpdate(attributeName, expressionParser.parse());

        if (aTokenizer->currentIsChar(';') || aTokenizer->currentIs(SqlKeywords::where_kw)) {
            break;
        }
        aTokenizer->check(",");
    }

    if (aTokenizer->currentIs(SqlKeywords::where_kw)) {
        aTokenizer->check(SqlKeywords::where_kw);
        ExpressionParser expressionParser(aTokenizer);
        theQuery->setWhereExpression(WhereExpression(expressionParser.parse()));
    } else {
        theQuery->setWhereExpression(WhereExpression());
    }

    aTokenizer->endBy(";");
    return new UpdateStatement(theQuery, output);
}

void makeWhereExpression(Tokenizer *aTokenizer, ExpressionParser &expressionParser, SQLQueryPtr &theQuery) {
    if (aTokenizer->currentIs(SqlKeywords::where_kw)) {
        aTokenizer->check(SqlKeywords::where_kw);
        theQuery->setWhereExpression(WhereExpression(expressionParser.parse()));
    } else {
        theQuery->setWhereExpression(WhereExpression());
    }
}


