#pragma once

#include <iostream>

class BinarySymbol {
public:
    uint64_t symbol;
    int16_t last_bit;

    BinarySymbol();
    explicit BinarySymbol(uint64_t bits, uint16_t size = SYMBOL_SIZE);
    void operator<<=(int shift);
    void operator>>=(int shift);
    bool operator[](size_t pos) const;
    bool operator<(const BinarySymbol& other) const;
    bool operator==(const BinarySymbol& other) const;
    size_t Size() const;

    struct Iterator {
        uint16_t pos;
        const BinarySymbol& symbol_ref;

        Iterator(const BinarySymbol& symbol, uint16_t pos) : pos(pos), symbol_ref(symbol) {
        }

        bool operator*();
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;  // NOLINT
    Iterator end() const;    // NOLINT

private:
    static const uint16_t SYMBOL_SIZE = 9;
};

namespace std {
template <>
struct hash<BinarySymbol> {
    size_t operator()(const BinarySymbol& k) const {
        size_t res = 17;
        res = res * 31 + hash<uint16_t>()(k.symbol);
        res = res * 31 + hash<uint16_t>()(k.last_bit);
        return res;
    }
};
};  // namespace std
