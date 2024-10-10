//
// Created by chiendd on 22/07/2024.
//

#include <iostream>
#include <sstream>
#include <string>

#include "core/controller/tokenizer/tokenizer.h"
#include "core/storage/expression/expression.h"
#include "core/controller/tokenizer/expression_parser.h"
#include "core/util/helpers.h"

using namespace Csql;

int main(int argc, const char * argv[]) {
    std::string text = "'abc' <> 'abc';";
    std::stringstream ss(text);

    try {
        Tokenizer tokenizer(ss);
        tokenizer.tokenize();

        Expression *expression = ExpressionParser(&tokenizer).parse();

        JoinedTuple joined_tuple;
        std::cout << Helpers::SqlTypeHandle::covertSqlTypeToString(expression->apply(joined_tuple)) << '\n';
    } catch (Errors e) {
        std::cout << e.what() << '\n';
    }
}

