#ifndef ARCHIVER_COMPRESSOR_
#define ARCHIVER_COMPRESSOR_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "bit_stream.h"
#include "constants.h"
#include "files.h"
#include "huffman.h"

class Compressor {
public:
    explicit Compressor(Path archive_name);

    void CompressFile(Path filename, bool is_last = false);

private:
    std::ifstream input_;
    std::ofstream os_;
    BitWriter output_;

    std::string current_file_;

    SymbolsCount symbols_count_;
    std::unordered_map<std::size_t, std::size_t> sizes_count_;
    CodeTable codes_;

    std::size_t max_size_;
    Alphabet alphabet_;

    void OpenFile(Path filename);
    void ResetPosition();

    void CountSymbols();
    void Reset();

    void WriteSymbol(Char symbol);
    void WriteNumber(std::size_t number);
    void WriteFile(bool is_last = true);
};

void Compress(Path archive_name, const std::vector<Path>& filenames);

#endif  // ARCHIVER_COMPRESSOR_