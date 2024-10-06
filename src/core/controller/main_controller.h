//
// Created by chiendd on 06/10/2024.
//

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "parsers/parser.h"
#include <vector>

namespace Csql {
    class MainController {
    public:
        explicit MainController(std::ostream& aOutput);
        std::string getOutput() const;
        Statement* parser(Tokenizer &aTokenizer);
    private:
        std::vector<Parser*> parsers;
        std::ostream &output;
    };
}

#endif //MAIN_CONTROLLER_H
