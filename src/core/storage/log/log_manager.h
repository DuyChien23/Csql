//
// Created by chiendd on 15/12/2024.
//

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H
#include <cstdint>
#include <functional>

#include "undo_entry.h"

using UndoEntryVisitor = std::function<void(UndoEntry*)>;

class LogManager {
public:
    static LogManager* getInstance() {
        if (instance == nullptr) {
            instance = new LogManager();
        }

        return instance;
    }

    void appendUndoEntry(std::string& databaseName, uint32_t _transactionId, OperationType _operationType,
                                const std::string &_tableName, const Tuple &_beforeImage, const Tuple &_afterImage);

    void eachUndoEntry(std::string& databaseName, uint32_t transactionId, const UndoEntryVisitor &aCallback);

private:
    LogManager();

    uint32_t getCurrentFileIndex(std::string& databaseName);

    std::fstream makeFstream(std::string& databaseName, uint32_t index, std::ios::openmode mode);

    std::string makePath(std::string& databaseName, uint32_t index);

    int countFile;

    static LogManager* instance;
};


#endif //LOG_MANAGER_H
