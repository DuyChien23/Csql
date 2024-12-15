//
// Created by chiendd on 31/10/2024.
//

#ifndef B_PLUS_KEY_H
#define B_PLUS_KEY_H
#include <cstdint>
#include <string>
#include <variant>
#include <vector>


enum KeyElementType {
    int_type = 'I',
    float_type = 'F',
    varchar_type = 'V'
};

using KeyElement = std::variant<int64_t, double, std::string>;

using PairKeyElement = std::pair<KeyElementType, KeyElement>;

class BPlusKey : public std::vector<PairKeyElement> {
public:
    BPlusKey() = default;

    template<typename T>
    BPlusKey(T key) {
        resize(0);
        add(key);
    }

    template<typename T>
    void add(T key) {
        if constexpr (std::is_same_v<T, double>) {
            push_back(std::make_pair(float_type, key));
        }
        if constexpr (std::is_arithmetic_v<T>) {
            push_back(std::make_pair(int_type, int64_t(key)));
        }
        if constexpr (std::is_same_v<T, std::string>) {
            push_back(std::make_pair(varchar_type, key));
        }
        if constexpr (std::is_same_v<T, const char *>) {
            push_back(std::make_pair(varchar_type, std::string(key)));
        }
    }

    uint32_t getRawSize();

    bool operator==(const BPlusKey &op) const;

    bool operator!=(const BPlusKey &op) const;

    bool operator<(const BPlusKey &op) const;

    bool operator<=(const BPlusKey &op) const;

    void log();
};

namespace std {
    template <>
    struct hash<KeyElement> {
        size_t operator()(const KeyElement& key) const {
            return std::visit([](const auto& value) {
                return std::hash<std::decay_t<decltype(value)>>()(value);
            }, key);
        }
    };

    template <>
    struct hash<PairKeyElement> {
        size_t operator()(const PairKeyElement& element) const {
            size_t hash1 = std::hash<KeyElementType>()(element.first);
            size_t hash2 = std::hash<KeyElement>()(element.second);
            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };

    template <>
    struct hash<BPlusKey> {
        size_t operator()(const BPlusKey& key) const {
            size_t hashValue = 0;
            std::hash<PairKeyElement> elementHash;
            for (const auto& element : key) {
                hashValue ^= elementHash(element) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            }
            return hashValue;
        }
    };
}

#endif //B_PLUS_KEY_H
