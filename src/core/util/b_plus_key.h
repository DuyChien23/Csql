//
// Created by chiendd on 31/10/2024.
//

#ifndef B_PLUS_KEY_H
#define B_PLUS_KEY_H
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace Csql {
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
            if constexpr (std::is_same_v<T, const char*>) {
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
}

#endif //B_PLUS_KEY_H
