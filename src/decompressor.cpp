#include "decompressor.h"

#include <cstddef>
#include <fstream>
#include <memory>
#include <vector>

#include "binary_trie.h"
#include "bit_stream.h"
#include "constants.h"
#include "exceptions.h"
#include "files.h"
#include "huffman.h"

Decompressor::Decompressor(Path filename) : is_(filename), input_(is_) {
}

void Decompressor::OpenFile(Path filename) {
    if (!ValidateOutput(filename)) {
        throw OutputError();
    }
    os_ = std::ofstream(filename);
}

void Decompressor::Reset() {
    codes_.clear();
    trie_ = std::make_shared<BinaryTrie>();
}

Char Decompressor::ReadNumber() {
    Char number = 0;
    try {
        number = input_.ReadBits<Char>(9);
    } catch (const BitReader::EndOfFile& ex) {
        throw InvalidFormat();
    }
    if (number > 259) {
        throw InvalidFormat();
    }
    return number;
}

void Decompressor::ReadHeader() {
    std::size_t count = ReadNumber();
    std::vector<Char> alphabet(count);
    for (Char& c : alphabet) {
        c = ReadNumber();
    }
    std::size_t current = 0;
    CodeSizes sizes(count);
    for (std::size_t size = 1; current < count; ++size) {
        std::size_t size_count = ReadNumber();
        if (current + size_count > count) {
            throw InvalidFormat();
        }
        for (std::size_t i = 0; i < size_count; ++i, ++current) {
            sizes[current] = {alphabet[current], size};
        }
    }
    codes_ = CanonicalCodes(sizes);
}

void Decompressor::GenerateTrie() {
    for (const auto& [key, code] : codes_) {
        try {
            trie_->AddCode(code.code, code.size, key);
        } catch (const BinaryTrie::CodeAlreadyExists& ex) {
            throw InvalidFormat();
        }
    }
}

Char Decompressor::ReadSymbol(const BinaryTrie& node) {
    if (node.IsTerminal()) {
        return node.key_;
    }
    if (node.IsLeaf()) {
        throw InvalidFormat();
    }
    bool bit = false;
    try {
        bit = input_.ReadBit();
    } catch (const BitReader::EndOfFile& ex) {
        throw InvalidFormat();
    }
    if (!bit) {
        return ReadSymbol(node.CLeft());
    } else {
        return ReadSymbol(node.CRight());
    }
}

Char Decompressor::ReadSymbol() {
    return ReadSymbol(*trie_);
}

std::string Decompressor::ReadFilename() {
    std::string filename;
    while (true) {
        Char symbol = ReadSymbol();
        if (symbol > FILENAME_END) {
            throw InvalidFormat();
        } else if (symbol == FILENAME_END) {
            return filename;
        } else {
            filename += static_cast<char>(symbol);
        }
    }
}

bool Decompressor::DecompressFile() {
    Reset();

    ReadHeader();
    GenerateTrie();

    Path filename = ReadFilename();
    OpenFile(filename);

    while (true) {
        Char symbol = ReadSymbol();
        if (symbol == FILENAME_END) {
            throw InvalidFormat();
        } else if (symbol == ONE_MORE_FILE) {
            return true;
        } else if (symbol == END_OF_ARCHIVE) {
            return false;
        } else {
            os_.put(static_cast<char>(symbol));
        }
    }
}

void Decompress(Path archive_name) {
    Decompressor decompressor(archive_name);
    while (decompressor.DecompressFile()) {
    }
}
