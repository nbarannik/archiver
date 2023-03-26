#include "out_binary_stream.h"

OutBinaryStream::OutBinaryStream(std::string_view file_name) {
    out_.open(static_cast<std::string>(file_name), std::ios::binary);
    if (!out_.is_open()) {
        throw std::runtime_error("File open error");
    }
}

void OutBinaryStream::operator<<(const BinarySymbol& c) {
    for (const auto& bit : c) {
        if (buff_size_ == BufferSize) {
            out_.write(buff_.data(), BufferSize);
            buff_size_ = 0;
            bit_pos_ = 0;
        }

        if ((buff_[buff_size_] >> (BYTE_SIZE - bit_pos_ - 1)) & 1) {
            size_t pos = (BYTE_SIZE - bit_pos_ - 1);
            buff_[buff_size_] = static_cast<char>(static_cast<int>(buff_[buff_size_]) ^ (1 << pos));
        }

        buff_[buff_size_] = static_cast<char>(buff_[buff_size_] | (bit << (BYTE_SIZE - bit_pos_ - 1)));
        ++bit_pos_;
        if (bit_pos_ == BYTE_SIZE) {
            bit_pos_ = 0;
            ++buff_size_;
        }
    }
}

OutBinaryStream::~OutBinaryStream() {
    Close();
}

void OutBinaryStream::Close() {
    if (buff_size_ != BufferSize) {
        if (buff_size_) {
            out_.write(buff_.data(), static_cast<std::streamsize>(buff_size_));
        }

        if (bit_pos_) {
            char correct_bit = 0;
            for (int bit = 0; bit < bit_pos_; ++bit) {
                bool previous_bit = (buff_[buff_size_] >> (BYTE_SIZE - bit - 1)) & 1;
                correct_bit = static_cast<char>(correct_bit | (previous_bit << (BYTE_SIZE - bit - 1)));
            }
            out_.write(&correct_bit, 1);
        }

        buff_size_ = 0;
        bit_pos_ = 0;
        out_.close();
    }
}