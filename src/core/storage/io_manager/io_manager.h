//
// Created by chiendd on 21/07/2024.
//

#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <memory>
#include <cstring>
#include <functional>
#include <vector>

#include "../../util/configs.h"
#include "../../util/types.h"

class IOManager {
public:
    IOManager() = default;

    virtual ~IOManager() = default;

    //Write data of block to disk
    void encode(std::fstream &anOutput);

    //Read data of block from disk
    void decode(std::fstream &anInput);

    void resetBufferOffset();

    char rawData[Configs::storageUnitSize] = {};

protected:
    //Data temp

    uint32_t bufferOffset = 0;

    //Write value to rawData
    template<typename T>
    void write(T aValue) {
        std::memcpy(rawData + bufferOffset, reinterpret_cast<char *>(&aValue), sizeof aValue);
        bufferOffset += sizeof aValue;
    }

    template<typename T>
    void write(const std::vector<T> &aValue) {
        write(aValue.size());
        for (const auto &value: aValue) {
            write(value);
        }
    }

    void write(const PairKeyElement &aValue);

    void write(const std::string &aString);

    void write(SqlTypes aValue, DataTypes type);

    //Read value from rawData
    template<typename T>
    void read(T &aValue) {
        std::memcpy(reinterpret_cast<char *>(&aValue), rawData + bufferOffset, sizeof aValue);
        bufferOffset += sizeof aValue;
    }

    template<typename T>
    void read(std::vector<T> &aValue) {
        size_t size;
        read(size);
        aValue.resize(size);
        for (auto &value: aValue) {
            read(value);
        }
    }

    void read(PairKeyElement &aValue);

    void read(std::string &aString);

    void read(SqlTypes &aValue, DataTypes type);

    void shift(uint32_t dest, uint32_t src, uint32_t range) {
        std::memcpy(rawData + dest, rawData + src, range);
    }
};


#endif //IO_MANAGER_H
