#pragma once

#include <fstream>
#include <string_view>
#include <array>
#include "binary_symbol.h"

class OutBinaryStream {
public:
    explicit OutBinaryStream(std::string_view file_name);
    ~OutBinaryStream();
    void Close();
    void operator<<(const BinarySymbol& c);
    const size_t BYTE_SIZE = 8;

private:
    static constexpr size_t BufferSize = 1024;
    std::ofstream out_;
    std::array<char, BufferSize> buff_;
    size_t buff_size_ = 0;
    size_t bit_pos_ = 0;
};