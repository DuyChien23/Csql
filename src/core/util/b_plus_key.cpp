//
// Created by chiendd on 31/10/2024.
//

#include "b_plus_key.h"

#include <assert.h>
#include <iostream>

namespace Csql {
    bool BPlusKey::operator==(const BPlusKey &op) const {
        assert(size() == op.size());
        for (int i = 0; i < size(); ++i) {
            assert(at(i).first == op.at(i).first);
            if (at(i).second != op.at(i).second) {
                return false;
            }
        }
        return true;
    }

    bool BPlusKey::operator!=(const BPlusKey &op) const {
        return !(*this == op);
    }

    bool BPlusKey::operator<(const BPlusKey &op) const {
        assert(size() == op.size());
        for (int i = 0; i < size(); ++i) {
            assert(at(i).first == op.at(i).first);
            if (at(i).second < op.at(i).second) {
                return true;
            }
            if (at(i).second > op.at(i).second) {
                return false;
            }
        }
        return false;
    }

    bool BPlusKey::operator<=(const BPlusKey &op) const {
        assert(size() == op.size());
        for (int i = 0; i < size(); ++i) {
            assert(at(i).first == op.at(i).first);
            if (at(i).second < op.at(i).second) {
                return true;
            }
            if (at(i).second > op.at(i).second) {
                return false;
            }
        }
        return true;
    }

    uint32_t BPlusKey::getRawSize() {
        uint32_t result = sizeof(size());
        for (auto& element : *this) {
            result += sizeof(element.first);
            switch (element.first) {
                case int_type:
                    result += sizeof(int64_t);
                    break;
                case float_type:
                    result += sizeof(double);
                    break;
                case varchar_type:
                    result += sizeof(uint32_t) + std::get<std::string>(element.second).size();
                    break;
            }
        }
        return result;
    }

    void BPlusKey::log() {
        std::cerr << "{";
        for (int i = 0; i < size(); ++i) {
            switch (at(i).first) {
                case int_type:
                    std::cerr << std::get<int64_t>(at(i).second);
                    break;
                case float_type:
                    std::cerr << std::get<double>(at(i).second);
                    break;
                case varchar_type:
                    std::cerr << std::get<std::string>(at(i).second);
                    break;
            }
            if (i + 1 < this->size()) std::cerr << ", ";
        }
        std::cerr << "}";
    }


}
