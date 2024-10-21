//
// Created by chiendd on 21/07/2024.
//

#include "io_manager.h"

#include <unistd.h>

#include "../../util/errors.h"
#include "../page/slotted_page.h"

namespace Csql {
    void IOManager::encode(std::fstream &anOutput) {
        anOutput.write(rawData, Configs::storageUnitSize);
        anOutput.flush();

        if (!anOutput.good()) throw Errors("Can't write data to disk");
    }

    void IOManager::decode(std::fstream &anInput) {
        anInput.read(rawData, Configs::storageUnitSize);

        if (!anInput.good()) throw Errors("Can't read data from disk");
    }

    void IOManager::write(const PairKeyElement &aValue) {
        write(aValue.first);

        if (aValue.first == int_type) {
            write(std::get<int64_t>(aValue.second));
        } else if (aValue.first == float_type) {
            write(std::get<double>(aValue.second));
        } else {
            write(std::get<std::string>(aValue.second));
        }
    }

    void IOManager::write(const std::string &aString) {
        uint32_t length = aString.size();
        write(length);
        std::memcpy(rawData + bufferOffset, aString.c_str(), length);
        bufferOffset += length;
    }

    void IOManager::write(SqlTypes aValue, DataTypes type) {
        switch (type) {
            case DataTypes::int_type: {
                SqlIntType value = std::get<SqlIntType>(aValue);
                write(value);
                break;
            }
            case DataTypes::bool_type: {
                SqlBoolType value = std::get<SqlBoolType>(aValue);
                write(value);
                break;
            }
            case DataTypes::datetime_type: {
                SqlDatetimeType value = std::get<SqlDatetimeType>(aValue);
                write(value);
                break;
            }
            case DataTypes::float_type: {
                SqlFloatType value = std::get<SqlFloatType>(aValue);
                write(value);
                break;
            }
            case DataTypes::varchar_type: {
                SqlVarcharType value = std::get<SqlVarcharType>(aValue);
                write(value);
                break;
            }
            case DataTypes::b_plus_key: {
                BPlusKey value = std::get<BPlusKey>(aValue);
                write(value.size());
                for (auto &element : value) {
                    write(element);
                }
                break;
            }
            default:
                break;
        }
    }

    void IOManager::read(PairKeyElement &aValue) {
        read(aValue.first);
        if (aValue.first == int_type) {
            int64_t value;
            read(value);
            aValue.second = value;
        } else if (aValue.first == float_type) {
            double value;
            read(value);
            aValue.second = value;
        } else {
            std::string value;
            read(value);
            aValue.second = value;
        }
    }


    void IOManager::read(std::string &aString) {
        uint32_t length;
        read(length);
        aString = std::string(rawData + bufferOffset, rawData + bufferOffset + length);
        bufferOffset += length;
    }

    void IOManager::read(SqlTypes& aValue, DataTypes type) {
        switch (type) {
            case DataTypes::int_type: {
                SqlIntType value;
                read(value);
                aValue = value;
                break;
            }
            case DataTypes::bool_type: {
                SqlBoolType value;
                read(value);
                aValue = value;
                break;
            }
            case DataTypes::datetime_type: {
                SqlDatetimeType value;
                read(value);
                aValue = value;
                break;
            }
            case DataTypes::float_type: {
                SqlFloatType value;
                read(value);
                aValue = value;
                break;
            }
            case DataTypes::varchar_type: {
                SqlVarcharType value;
                read(value);
                aValue = value;
                break;
            }
            case DataTypes::b_plus_key: {
                BPlusKey value;
                size_t sz = 0;
                read(sz);
                value.resize(sz);
                for (auto &element : value) {
                    read(element);
                }
                aValue = value;
                break;
            }
            default:
                break;
        }
    }

    void IOManager::resetBufferOffset() {
        this->bufferOffset = 0;
    }

}
