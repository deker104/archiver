#include "compressor.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <memory>
#include <string_view>

#include "binary_trie.h"
#include "constants.h"
#include "huffman.h"
#include "priority_queue.h"

Compressor::Compressor(std::filesystem::path archive_name) : os_(archive_name, std::ios::binary), output_(os_) {
}

void Compressor::Reset() {
    current_file_.clear();
    symbols_count_.clear();
    sizes_count_.clear();
    codes_.clear();
    alphabet_.clear();
    max_size_ = 0;
}

void Compressor::OpenFile(std::filesystem::path filename) {
    current_file_ = filename.filename();
    input_ = std::ifstream(filename, std::ios::binary);
}

void Compressor::ResetPosition() {
    input_.clear();
    input_.seekg(0);
}

void Compressor::CountSymbols() {
    symbols_count_[FILENAME_END] = 1;
    symbols_count_[ONE_MORE_FILE] = 1;
    symbols_count_[END_OF_ARCHIVE] = 1;

    for (char c : current_file_) {
        ++symbols_count_[c];
    }

    while (!input_.eof()) {
        unsigned char c = input_.get();
        if (input_.eof()) {
            break;
        }
        ++symbols_count_[c];
    }
}

void Compressor::WriteSymbol(Char symbol) {
    const auto& [code, size] = codes_[symbol];
    output_.WriteBits(code, size);
}

void Compressor::WriteFile(bool is_last) {
    WriteNumber(symbols_count_.size());
    for (Char c : alphabet_) {
        WriteNumber(c);
    }
    for (std::size_t size = 1; size <= max_size_; ++size) {
        WriteNumber(sizes_count_[size]);
    }

    for (char c : current_file_) {
        WriteSymbol(c);
    }
    WriteSymbol(FILENAME_END);

    while (!input_.eof()) {
        unsigned char c = input_.get();
        if (input_.eof()) {
            break;
        }
        WriteSymbol(c);
    }
    if (is_last) {
        WriteSymbol(END_OF_ARCHIVE);
    } else {
        WriteSymbol(ONE_MORE_FILE);
    }
}

void Compressor::CompressFile(Path filename, bool is_last) {
    Reset();
    OpenFile(filename);
    CountSymbols();

    auto sizes = HuffmanEncoding(symbols_count_);
    alphabet_.reserve(sizes.size());
    for (const auto& [key, size] : sizes) {
        ++sizes_count_[size];
        alphabet_.push_back(key);
        max_size_ = size;
    }
    codes_ = CanonicalCodes(sizes);

    ResetPosition();
    WriteFile(is_last);
}

void Compress(Path archive_name, const std::vector<Path>& filenames) {
    Compressor compressor(archive_name);
    for (std::size_t i = 0; i < filenames.size(); ++i) {
        const auto& filename = filenames[i];
        bool is_last = (i == filenames.size() - 1);
        compressor.CompressFile(filename, is_last);
    }
}

void Compressor::WriteNumber(std::size_t number) {
    output_.WriteBits(number, 9);
}
