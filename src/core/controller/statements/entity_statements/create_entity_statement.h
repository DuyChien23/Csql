//
// Created by chiendd on 28/09/2024.
//

#ifndef CREATE_STATEMENT_H
#define CREATE_STATEMENT_H
#include "../statement.h"
#include "../../../storage/entity.h"

namespace Csql {
    class CreateEntityStatement : public Statement {
    public:
        CreateEntityStatement(SharedEntityPtr& aEntity) : entity(aEntity) {}
        void execute() override {}
    protected:
        SharedEntityPtr entity;
    };
}

#endif //CREATE_STATEMENT_H
