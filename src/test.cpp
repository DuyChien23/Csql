//
// Created by chiendd on 22/07/2024.
//


#include <iostream>

#include "core/storage/global_variable/global_variable.h"

int main(int argc, const char *argv[]) {
    std::cout << GlobalVariable::getInstance()->getTransactionCounter() << std::endl;
}

