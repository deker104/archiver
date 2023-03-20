#ifndef ARCHIVER_HUFFMAN_
#define ARCHIVER_HUFFMAN_

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "binary_trie.h"
#include "constants.h"

struct Code {
    std::size_t code;
    std::size_t size;
};

struct CodeSize {
    Char key;
    std::size_t size;

    bool operator<(const CodeSize& other) const;
};

using Alphabet = std::vector<Char>;
using SymbolsCount = std::unordered_map<Char, std::size_t>;
using CodeSizes = std::vector<CodeSize>;
using CodeTable = std::unordered_map<Char, Code>;

bool PointerCompare(const BinaryTrie::Pointer& lhs, const BinaryTrie::Pointer& rhs);

CodeSizes HuffmanEncoding(const SymbolsCount& symbols_count);
CodeTable CanonicalCodes(const CodeSizes& sizes);

#endif  // ARCHIVER_HUFFMAN_
