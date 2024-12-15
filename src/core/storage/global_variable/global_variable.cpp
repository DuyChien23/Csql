//
// Created by chiendd on 17/12/2024.
//

#include <fstream>

#include "global_variable.h"

const std::string GLOBAL_VARIABLE_FILE = "global_variable.json";
const int DEFAULT_TRANSACTION_COUNTER = 1000000;
GlobalVariable* GlobalVariable::instance = nullptr;

int GlobalVariable::getTransactionCounter() {
    std::lock_guard lock(mtx);
    load();
    int result = get<int>("transaction_counter", DEFAULT_TRANSACTION_COUNTER) + 1;
    set("transaction_counter", result);
    save();
    return result;
}

void GlobalVariable::load() {
    std::ifstream file(GLOBAL_VARIABLE_FILE);
    if (file.is_open()) {
        file >> configData;
        file.close();
    } else {
        configData = json({});
    }
}

void GlobalVariable::save() {
    std::ofstream file(GLOBAL_VARIABLE_FILE);
    file << configData.dump(4);
    file.close();
}
