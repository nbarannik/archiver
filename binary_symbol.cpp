#include "binary_symbol.h"

BinarySymbol::BinarySymbol(uint64_t bits, uint16_t size) {
    symbol = bits;
    last_bit = size - 1;
}

BinarySymbol::BinarySymbol() {
    symbol = 0;
    last_bit = -1;
}

void BinarySymbol::operator<<=(int shift) {
    last_bit += shift;
    symbol <<= shift;
}

void BinarySymbol::operator>>=(int shift) {
    last_bit -= shift;
    symbol >>= shift;
}

size_t BinarySymbol::Size() const {
    return last_bit + 1;
}

bool BinarySymbol::operator[](size_t pos) const {
    return (symbol >> (last_bit - pos)) & 1;
}

bool BinarySymbol::operator==(const BinarySymbol& other) const {
    return (symbol == other.symbol and last_bit == other.last_bit);
}

bool BinarySymbol::operator<(const BinarySymbol& other) const {
    return symbol < other.symbol;
}

bool BinarySymbol::Iterator::operator*() {
    return symbol_ref[pos];
}

BinarySymbol::Iterator& BinarySymbol::Iterator::operator++() {
    ++pos;
    return *this;
}

bool BinarySymbol::Iterator::operator!=(const Iterator& other) const {
    return pos != other.pos;
}

BinarySymbol::Iterator BinarySymbol::begin() const {
    return Iterator(*this, 0);
}

BinarySymbol::Iterator BinarySymbol::end() const {
    return Iterator(*this, last_bit + 1);
}