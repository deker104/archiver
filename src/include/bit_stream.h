#ifndef ARCHIVER_BIT_STREAM_
#define ARCHIVER_BIT_STREAM_

#include <cstddef>
#include <cstdint>
#include <exception>
#include <ios>
#include <iostream>

class BitReader {
    const static std::streamsize BUFFER_CAPACITY = 1 << 12;

public:
    class EndOfFile : public std::exception {};

    explicit BitReader(std::istream& is);

    bool ReadBit();

    template <typename T>
    T ReadBits(std::size_t count) {
        T bits = 0;
        for (std::size_t i = 0; i < count; ++i) {
            std::size_t bit_position = count - i - 1;
            bits |= static_cast<T>(ReadBit()) << bit_position;
        }
        return bits;
    }

    bool Eof() const;

private:
    char buffer_[BUFFER_CAPACITY + 1];
    std::streamsize buffer_pos_ = 0;
    std::streamsize buffer_size_ = 0;
    int current_bit_ = 0;

    std::istream& is_;

    void BufferFill();
};

class BitWriter {
    const static std::streamsize BUFFER_SIZE = 1 << 12;

public:
    explicit BitWriter(std::ostream& is);
    ~BitWriter();

    void WriteBit(bool val);

    template <typename T>
    void WriteBits(T data, std::size_t count) {
        for (std::size_t i = 0; i < count; ++i) {
            std::size_t bit_position = count - i - 1;
            WriteBit((data >> bit_position) & 1);
        }
    }

private:
    char buffer_[BUFFER_SIZE] = {0};
    std::streamsize buffer_pos_ = 0;
    int current_bit_ = 0;

    std::ostream& os_;

    void BufferFlush();
};

#endif  // ARCHIVER_BIT_STREAM_