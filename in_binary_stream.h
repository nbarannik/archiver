#pragma once

#include <fstream>
#include <string_view>
#include <array>
#include <iostream>
#include "binary_symbol.h"

static const size_t BYTE_SIZE = 8;

class InBinaryStream {
public:
    explicit InBinaryStream(std::string_view full_file_name);

    bool operator>>(char& c);
    bool operator>>(BinarySymbol& symbol);
    std::string_view file_name;
    void Reset();

private:
    bool BufferRead();
    std::string_view full_file_name_;
    static constexpr size_t BufferSize = 1024;
    std::ifstream in_;
    std::array<char, BufferSize> buff_;
    size_t buff_pos_ = BufferSize;
    size_t buff_size_ = 0;
    size_t bit_pos_ = 0;
};