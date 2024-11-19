//
// Created by chiendd on 26/09/2024.
//

#include "sql_parser.h"

#include "../../storage/entity.h"
#include "../../storage/expression/value_expression.h"
#include "../statements/table_statements/create_table_statement.h"
#include "../statements/table_statements/delete_tuples_statement.h"
#include "../statements/table_statements/describe_table_statement.h"
#include "../statements/table_statements/drop_table_statement.h"
#include "../statements/table_statements/insert_statement.h"
#include "../statements/table_statements/select_statement.h"
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
                    case SqlKeywords::not_kw:
                        aTokenizer->check(SqlKeywords::null_kw);
                        theAtrribute->setNullable(false);
                        break;
                    default:
                        return nullptr;
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
            std::string s1, s2, columnName = "";

            aTokenizer->consumeAttribute(s1, s2);

            if (aTokenizer->currentIs(SqlKeywords::as_kw)) {
                aTokenizer->check(SqlKeywords::as_kw);
                aTokenizer->consumeType(columnName);
            }

            theQuery->addTaget(s1, s2, columnName);

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
        aTokenizer->consumeAttribute(tableName, attributeName);
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
        currentIs(SqlKeywords::right_kw)) {
        SqlKeywords joinType;
        std::string entityName;
        aTokenizer->consumeType(joinType)->check(SqlKeywords::join_kw)->consumeType(entityName)->check(
            SqlKeywords::on_kw);
        theQuery->addJoinExpression(JoinExpression(expressionParser.parse()));
        return false;
    }
}

Statement *SqlParser::deleteTuplesStatement(Tokenizer *aTokenizer) {
    SQLQueryPtr theQuery = std::make_unique<SQLQuery>();

    std::string entityName;
    aTokenizer->check(SqlKeywords::delete_kw)->check(SqlKeywords::from_kw)->consumeType(entityName);
    theQuery->setEntityName(entityName);

    ExpressionParser expressionParser(aTokenizer);
    makeWhereExpression(aTokenizer, expressionParser, theQuery);

    return new DeleteTuplesStatement(theQuery, output);
}

void makeWhereExpression(Tokenizer *aTokenizer, ExpressionParser &expressionParser, SQLQueryPtr &theQuery) {
    if (aTokenizer->currentIs(SqlKeywords::where_kw)) {
        aTokenizer->check(SqlKeywords::where_kw);
        theQuery->setWhereExpression(WhereExpression(expressionParser.parse()));
    } else {
        theQuery->setWhereExpression(WhereExpression());
    }
}
