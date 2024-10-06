//
// Created by chiendd on 26/09/2024.
//

#include "sql_parser.h"

#include <iostream>

#include "../../storage/entity.h"
#include "../statements/entity_statements/create_table_statement.h"
#include "../statements/entity_statements/describe_table_statement.h"
#include "../statements/entity_statements/drop_table_statement.h"
#include "../statements/entity_statements/insert_statement.h"

namespace Csql {
    void getListString(Tokenizer *aTokenizer, std::vector<std::string>& list);

    //-------------------------------------------------------------------------------------------------
    Statement* SqlParser::makeStatement(Tokenizer &aTokenizer) {
        for (auto &factory : factories) {
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

    Statement* SqlParser::createEntityStatement(Tokenizer *aTokenizer) {
        std::string theEntityName;
        aTokenizer->check(SqlKeywords::create_kw)->check(SqlKeywords::table_kw)->skipType(theEntityName)->check("(");

        SharedEntityPtr theEntity = std::make_shared<Entity>(theEntityName);
        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> foreignKeys;

        while (!aTokenizer->currentIsChar(')')) {
            if (aTokenizer->currentIs(SqlKeywords::foreign_kw)) {
                std::string attributeName;
                aTokenizer->check(SqlKeywords::foreign_kw)->check(SqlKeywords::key_kw)->check("(")->skipType(attributeName)->check(")");
                std::string refEntityName, refAttributeName;
                aTokenizer->check(SqlKeywords::references_kw)->skipType(refEntityName)->check("(")->skipType(refAttributeName)->check(")");
                foreignKeys.push_back({attributeName, {refEntityName, refAttributeName}});
            } else {
                Attribute *theAtrribute = new Attribute();
                std::string attributeName, type;
                aTokenizer->skipType(attributeName)->skipType(type);
                theAtrribute->setName(attributeName);
                theAtrribute->setType(Helpers::SqlTypeHandle::covertStringToDataType(type));

                if (theAtrribute->getType() == DataTypes::varchar_type) {
                    int size;
                    aTokenizer->check("(")->skipType(size)->check(")");
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
                    aTokenizer->skipType(theKeyword);
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

        for (auto foreignKey : foreignKeys) {
            auto theAttribute = theEntity->getAttribute(foreignKey.first);
            if (!theAttribute) throw Errors("Reference to non-existing attribute");

            theEntity->getAttribute(foreignKey.first)->setReference(
                (new Reference)->setEntityName(foreignKey.second.first)->setAttributeName(foreignKey.second.second)
            );
        }

        return new CreateTableStatement(theEntity, output);
    }

    Statement* SqlParser::describeEntityStatement(Tokenizer *aTokenizer) {
        std::string theEntityName;
        aTokenizer->check(SqlKeywords::describe_kw)->check(SqlKeywords::table_kw)->skipType(theEntityName)->endBy(";");

        return new DescribeTableStatement(theEntityName, output);
    }

    Statement* SqlParser::dropEntityStatement(Tokenizer *aTokenizer) {
        std::string theEntityName;
        aTokenizer->check(SqlKeywords::drop_kw)->check(SqlKeywords::table_kw)->skipType(theEntityName)->endBy(";");

        return new DropTableStatement(theEntityName, output);
    }

    Statement *SqlParser::insertStatement(Tokenizer *aTokenizer) {
        std::string theEntityName;
        aTokenizer->check(SqlKeywords::insert_kw)->check(SqlKeywords::into_kw)->skipType(theEntityName);

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
    void getListString(Tokenizer *aTokenizer, std::vector<std::string>& list) {
        aTokenizer->check("(");
        while (!aTokenizer->currentIsChar(')')) {
            std::string attributeName;
            aTokenizer->skipType(attributeName);

            list.push_back(attributeName);
            if (aTokenizer->currentIsChar(')')) {
                break;
            }
            aTokenizer->check(",");
        }
        aTokenizer->check(")");
    }
}
