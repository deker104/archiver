#include "bit_stream.h"

#include <algorithm>
#include <climits>
#include <iostream>
#include <iterator>

BitReader::BitReader(std::istream& is) : is_(is) {
}

void BitReader::BufferFill() {
    if (buffer_pos_ != buffer_size_) {
        return;
    }
    if (Eof()) {
        buffer_pos_ = 0;
        buffer_size_ = 0;
        return;
    }
    is_.read(buffer_, BUFFER_CAPACITY);
    buffer_size_ = is_.gcount();
    buffer_pos_ = 0;
}

bool BitReader::Eof() const {
    return buffer_pos_ == buffer_size_ && is_.eof();
}

bool BitReader::ReadBit() {
    if (Eof()) {
        throw EndOfFile();
    }
    if (buffer_pos_ == buffer_size_) {
        BufferFill();
    }
    int bit_position = CHAR_BIT - current_bit_ - 1;
    bool bit = (buffer_[buffer_pos_] >> bit_position) & 1;
    ++current_bit_;
    if (current_bit_ == CHAR_BIT) {
        current_bit_ = 0;
        ++buffer_pos_;
    }
    return bit;
}

BitWriter::BitWriter(std::ostream& os) : os_(os) {
}

BitWriter::~BitWriter() {
    BufferFlush();
}

void BitWriter::BufferFlush() {
    os_.write(buffer_, buffer_pos_ + (current_bit_ > 0));
    buffer_pos_ = 0;
    current_bit_ = 0;
    std::fill(std::begin(buffer_), std::end(buffer_), 0);
}

void BitWriter::WriteBit(bool val) {
    if (buffer_pos_ == BUFFER_SIZE) {
        BufferFlush();
    }
    int8_t bit_position = CHAR_BIT - current_bit_ - 1;
    buffer_[buffer_pos_] |= static_cast<char>(val << bit_position);  // NOLINT
    ++current_bit_;
    if (current_bit_ == CHAR_BIT) {
        current_bit_ = 0;
        ++buffer_pos_;
    }
}
