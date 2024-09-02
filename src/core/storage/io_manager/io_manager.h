//
// Created by chiendd on 21/07/2024.
//

#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <fstream>
#include <memory>
#include <cstring>

#include "../../util/configs.h"
#include "../../util/types.h"

namespace Csql {
    class IOManager {
    public:
        IOManager() = default;

        ~IOManager() = default;

        //Write data of block to disk
        void encode(std::fstream& anOutput);

        //Read data of block from disk
        void decode(std::fstream& anInput);

        //Refreshes the variables based on the raw data array
        void refresh() {};

        //Saves variables to the raw data array
        void save() {};

        void resetBufferOffset();

    protected:
        //Data temp
        char rawData[Configs::storageUnitSize];

        uint32_t bufferOffset = 0;

        //Write value to rawData
        template<typename T>
        void write(T aValue) {
            std::memcpy(rawData + bufferOffset, reinterpret_cast<char*>(&aValue), sizeof aValue);
            bufferOffset += sizeof aValue;
        }

        void write(const std::string& aString);

        void write(SqlTypes aValue, DataTypes type);

        //Read value from rawData
        template<typename T>
        void read(T& aValue) {
            std::memcpy(reinterpret_cast<char*>(&aValue), rawData + bufferOffset, sizeof aValue);
            bufferOffset += sizeof aValue;
        }

        void read(std::string& aString);

        void read(SqlTypes &aValue, DataTypes type);
    };
}

#endif //IO_MANAGER_H
