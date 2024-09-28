//
// Created by chiendd on 26/09/2024.
//

#include "sql_parser.h"

#include "../../storage/entity.h"
#include "../statements/entity_statements/create_entity_statement.h"

namespace Csql {
    Statement* SqlParser::makeStatement(Tokenizer &aTokenizer) {
        for (auto &theFactory : factories) {
            Statement *theStatement = (this->*theFactory)(aTokenizer.reset());
            if (theStatement != nullptr) {
                return theStatement;
            }
        }
        return nullptr;
    }

    Statement* SqlParser::createEntityStatement(Tokenizer *aTokenizer) {
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::create_kw)->check(SqlKeywords::table_kw));
        RETURN_IF_CONDITION_FALSE(aTokenizer->peek());

        std::string theEntityName;
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(theEntityName));
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipData("("));

        SharedEntityPtr theEntity = std::make_shared<Entity>("noname", theEntityName);

        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> foreignKeys;

        while (!aTokenizer->currentIsChar(')')) {
            if (aTokenizer->currentIs(SqlKeywords::foreign_kw)) {
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipKeyword(SqlKeywords::foreign_kw));
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipKeyword(SqlKeywords::key_kw));
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipData("("));
                std::string attributeName;
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(attributeName));
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipData(")"));
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipKeyword(SqlKeywords::references_kw));
                std::string refEntityName;
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(refEntityName));
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipData("("));
                std::string refAttributeName;
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(refAttributeName));
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipData(")"));
                foreignKeys.push_back({attributeName, {refEntityName, refAttributeName}});
            } else {
                Attribute *theAtrribute = new Attribute();
                std::string attributeName;
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(attributeName));
                theAtrribute->setName(attributeName);

                std::string type;
                RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(type));
                theAtrribute->setType(Helpers::SqlTypeHandle::covertStringToDataType(type));
                if (theAtrribute->getType() == DataTypes::varchar_type) {
                    RETURN_IF_CONDITION_FALSE(aTokenizer->skipData("("));
                    int size;
                    RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(size));
                    RETURN_IF_CONDITION_FALSE(aTokenizer->skipData(")"));
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
                    RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(theKeyword));
                    switch (theKeyword) {
                        case SqlKeywords::primary_kw:
                            RETURN_IF_CONDITION_FALSE(aTokenizer->skipKeyword(SqlKeywords::key_kw));
                        theAtrribute->setPrimary(true);
                        break;
                        case SqlKeywords::auto_increment:
                            theAtrribute->setAutoIncrement(true);
                        break;
                        case SqlKeywords::unique_kw:
                            theAtrribute->setUnique(true);
                        break;
                        case SqlKeywords::not_kw:
                            RETURN_IF_CONDITION_FALSE(aTokenizer->skipKeyword(SqlKeywords::null_kw));
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
            RETURN_IF_CONDITION_FALSE(aTokenizer->skipData(","));
        }
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipData(")"));
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipData(";"));

        for (auto foreignKey : foreignKeys) {
            auto theAttribute = theEntity->getAttribute(foreignKey.first);
            if (!theAttribute) throw Errors("Reference to non-existing attribute");

            theEntity->getAttribute(foreignKey.first)->setReference(
                (new Reference)->setEntityName(foreignKey.second.first)->setAttributeName(foreignKey.second.second)
            );
        }

        return new CreateEntityStatement(theEntity, output);
    }
}
