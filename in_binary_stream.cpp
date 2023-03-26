#include "in_binary_stream.h"

std::string_view GetFileName(std::string_view full_name);

InBinaryStream::InBinaryStream(std::string_view path_file_name) {
    full_file_name_ = path_file_name;
    file_name = GetFileName(full_file_name_);
    in_.open(static_cast<std::string>(full_file_name_), std::ios::binary);
    if (!in_.is_open()) {
        throw std::runtime_error("File open error");
    }
}

void InBinaryStream::Reset() {
    in_.clear();
    in_.seekg(0, std::ios::beg);
    buff_pos_ = BufferSize;
    buff_size_ = 0;
}

bool InBinaryStream::BufferRead() {
    if (in_.eof()) {
        return false;
    }

    in_.read(buff_.data(), BufferSize);
    buff_pos_ = 0;
    buff_size_ = in_.gcount();
    bit_pos_ = 0;
    return true;
}

bool InBinaryStream::operator>>(char& c) {
    if (buff_pos_ == BufferSize) {
        if (!BufferRead()) {
            return false;
        }
    }

    if (buff_pos_ < buff_size_) {
        c = buff_[buff_pos_];
        ++buff_pos_;
        return true;
    }

    return false;
}

bool InBinaryStream::operator>>(BinarySymbol& symbol) {
    for (size_t bit = 0; bit < symbol.Size(); ++bit) {
        if (buff_pos_ == BufferSize) {
            if (!BufferRead()) {
                return false;
            }
        }

        if (buff_pos_ > buff_size_) {
            return false;
        }

        bool buff_bit = (buff_[buff_pos_] >> (BYTE_SIZE - bit_pos_ - 1)) & 1;
        bit_pos_++;
        if (bit_pos_ == BYTE_SIZE) {
            buff_pos_++;
            bit_pos_ = 0;
        }

        symbol.symbol |= (buff_bit << (symbol.Size() - bit - 1));
    }

    return true;
}