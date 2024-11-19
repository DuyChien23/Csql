//
// Created by chiendd on 27/09/2024.
//

#ifndef STATEMENT_H
#define STATEMENT_H
#include <ostream>

class Statement {
public:
    explicit Statement(std::ostream &aOutput) : output(aOutput) {
    }

    virtual ~Statement() = default;

    virtual void execute() = 0;

protected:
    std::ostream &output;
};


#endif //STATEMENT_H
