//
// Created by chiendd on 27/09/2024.
//

#ifndef STATEMENT_H
#define STATEMENT_H

namespace Csql {
    class Statement {
    public:
        virtual ~Statement() = default;
        virtual void execute() = 0;
    };
}

#endif //STATEMENT_H
