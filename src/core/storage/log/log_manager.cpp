//
// Created by chiendd on 15/12/2024.
//

#include "log_manager.h"

#include <iostream>

#include "../expression/other_expression.h"
#include "../../util/helpers.h"

LogManager* LogManager::instance = nullptr;

LogManager::LogManager() {
    countFile = 0;
}

void LogManager::appendUndoEntry(std::string& databaseName, uint32_t transactionId, OperationType operationType,
                                 const std::string &tableName, const Tuple &beforeImage, const Tuple &afterImage) {
    auto calcRawSize = [&] {
        uint32_t sz = 0;
        sz += sizeof transactionId;
        sz += sizeof operationType;
        sz += sizeof (uint32_t) + tableName.size();

        sz += sizeof (DataTypes) * (beforeImage.size() + afterImage.size());

        sz += sizeof (uint32_t);

        sz += Helpers::SqlTypeHandle::sizeOfTuple(beforeImage);
        sz += Helpers::SqlTypeHandle::sizeOfTuple(afterImage);
        return sz;
    };

    auto output = makeFstream(databaseName, getCurrentFileIndex(databaseName), std::ios::app);
    UndoEntry* entry = new UndoEntry(transactionId, operationType, tableName, beforeImage, afterImage, calcRawSize());
    entry->save();
    entry->encode(output);
    output.write(reinterpret_cast<char *>(&entry->rawSize), sizeof entry->rawSize);
    output.flush();
    delete entry;
}

void LogManager::eachUndoEntry(std::string &databaseName, uint32_t transactionId, const UndoEntryVisitor &aCallback) {
    for (auto index = getCurrentFileIndex(databaseName); index > 0; --index) {
        auto input = makeFstream(databaseName, index, std::ios::in);
        input.seekg(0, std::ios::end);

        while (input.tellg() > 0) {
            input.seekg(-sizeof(uint32_t), std::ios::cur);

            uint32_t size;
            input.read(reinterpret_cast<char *>(&size), sizeof size);
            input.seekg(-int(size + sizeof size), std::ios::cur);

            UndoEntry* entry = new UndoEntry(size);
            entry->decode(input);
            entry->refresh();
            if (entry->transactionId == transactionId) {
                if (entry->operationType == OperationType::BEGIN) {
                    return;
                }
                aCallback(entry);
            }

            delete entry;

            input.seekg(-int(size), std::ios::cur);
        }
    }
}

uint32_t LogManager::getCurrentFileIndex(std::string &databaseName) {
    auto thePath = Configs::undoLogDictionary + "/" + databaseName;

    if (countFile == 0) {
        if (!std::filesystem::exists(Configs::undoLogDictionary)) {
            std::filesystem::create_directories(Configs::undoLogDictionary);
        }

        if (!std::filesystem::exists(thePath)) {
            std::filesystem::create_directories(thePath);
        }

        Helpers::FolderHandle::eachFolder(thePath, [&](const std::string& _) {
            countFile++;
        });
    }

    if (countFile == 0 || std::filesystem::file_size(makePath(databaseName, countFile)) > Configs::maxUndoLogFileSize) {
        countFile++;
        makeFstream(databaseName, countFile, std::ios::out);
    }

    return countFile;
}

std::fstream LogManager::makeFstream(std::string &databaseName, uint32_t index, std::ios::openmode mode) {
    return std::fstream(makePath(databaseName, index), mode);
}


std::string LogManager::makePath(std::string &databaseName, uint32_t index) {
    return Configs::undoLogDictionary + "/" + databaseName + "/undo_log_" + std::to_string(index - 1) + ".log";
}
