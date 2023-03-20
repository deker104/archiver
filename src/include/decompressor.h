#ifndef ARCHIVER_DECOMPRESSOR_
#define ARCHIVER_DECOMPRESSOR_

#include <fstream>

#include "binary_trie.h"
#include "bit_stream.h"
#include "constants.h"
#include "files.h"
#include "huffman.h"

class Decompressor {
public:
    explicit Decompressor(Path archive_name);

    bool DecompressFile();

private:
    std::ifstream is_;
    BitReader input_;
    std::ofstream os_;

    CodeTable codes_;
    BinaryTrie::Pointer trie_;

    void OpenFile(Path filename);
    void Reset();

    Char ReadNumber();
    Char ReadSymbol();
    Char ReadSymbol(const BinaryTrie& node);
    void ReadHeader();
    std::string ReadFilename();

    void GenerateTrie();

    void WriteFile();
};

void Decompress(Path archive_name);

#endif  // ARCHIVER_DECOMPRESSOR_