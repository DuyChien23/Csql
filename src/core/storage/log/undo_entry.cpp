//
// Created by chiendd on 16/12/2024.
//

#include "undo_entry.h"

#include <assert.h>

#include "../attribute.h"
#include "../../util/helpers.h"

UndoEntry::UndoEntry(uint32_t _rawSize) : IOManager(_rawSize) {
}

UndoEntry::UndoEntry(uint32_t _transactionId, OperationType _operationType, const std::string &_tableName,
                     const Tuple &_beforeImage, const Tuple &_afterImage, uint32_t _rawSize) : transactionId(_transactionId),
                                                                            operationType(_operationType),
                                                                            tableName(_tableName),
                                                                            beforeImage(_beforeImage),
                                                                            afterImage(_afterImage),
                                                                            IOManager(_rawSize) {
};


void UndoEntry::refresh() {
    bufferOffset = 0;
    read(transactionId);
    read(operationType);
    read(tableName);
    read(bufferPoint);

    readTuple(beforeImage, bufferPoint);
    assert(bufferOffset == bufferPoint);
    readTuple(afterImage, rawSize);
    assert(bufferOffset == rawSize);
}

void UndoEntry::readTuple(Tuple &tuple, uint32_t endOffset) {
    while (bufferOffset < endOffset) {
        std::string name;
        read(name);
        SqlTypes aValue;
        DataTypes type;

        read(type);
        read(aValue, type);

        tuple.insert(std::make_pair(name, aValue));
    }
}

void UndoEntry::save() {
    bufferOffset = 0;
    write(transactionId);
    write(operationType);
    write(tableName);

    auto t = bufferOffset;
    bufferOffset += sizeof bufferPoint;

    writeTuple(beforeImage);
    bufferPoint = bufferOffset;

    writeTuple(afterImage);
    assert(bufferOffset == rawSize);

    bufferOffset = t;
    write(bufferPoint);
}

uint32_t UndoEntry::writeTuple(Tuple &tuple) {
    for (auto &p: tuple) {
        write(p.first);
        auto type = Helpers::SqlTypeHandle::covertSqlValueToDataTypes(p.second);
        write(type);
        write(p.second, type);
    }
}

