//
// Created by chiendd on 21/07/2024.
//

#ifndef ERRORS_H
#define ERRORS_H
#include <sstream>

namespace Csql {
    class Errors : public std::exception {
    public:
        explicit Errors(std::string _reason) : reason(_reason) {};

        const char* what() const noexcept override {
            return reason.c_str();
        }
    protected:
        std::string reason;
    };
}

#endif //ERRORS_H
