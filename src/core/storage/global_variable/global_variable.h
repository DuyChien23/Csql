//
// Created by chiendd on 17/12/2024.
//

#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

#include <mutex>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class GlobalVariable {
public:
    static GlobalVariable* getInstance() {
        if (instance == nullptr) {
            instance = new GlobalVariable();
        }
        return instance;
    };

    int getTransactionCounter();

private:
    GlobalVariable() = default;
    json configData;
    std::mutex mtx;

    static GlobalVariable* instance;

    void load();
    void save();

    template<typename T>
    T get(const std::string& key, const T& defaultValue) {
        if (configData.contains(key)) {
            return configData[key].get<T>();
        }
        return defaultValue;
    }

    template<typename T>
    void set(const std::string& key, const T& value) {
        configData[key] = value;
    }
};



#endif //GLOBAL_VARIABLE_H
