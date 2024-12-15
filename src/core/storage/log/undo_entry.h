//
// Created by chiendd on 16/12/2024.
//

#ifndef UNDO_ENTRY_H
#define UNDO_ENTRY_H
#include "../io_manager/io_manager.h"

enum class OperationType {
    INSERT = 'I',
    DELETE = 'D',
    UPDATE = 'U',
    BEGIN = 'B',
};

class UndoEntry : public IOManager {
public:
    UndoEntry(uint32_t rawSize);

    UndoEntry(uint32_t _transactionId, OperationType _operationType, const std::string &_tableName,
              const Tuple &_beforeImage, const Tuple &_afterImage, uint32_t _rawSize);

    void refresh();

    void save();

    uint32_t transactionId;
    OperationType operationType;
    std::string tableName;
    uint32_t bufferPoint;
    Tuple beforeImage;
    Tuple afterImage;

private:
    void readTuple(Tuple &tuple, uint32_t endOffset);

    uint32_t writeTuple(Tuple &tuple);
};


#endif //UNDO_ENTRY_H
