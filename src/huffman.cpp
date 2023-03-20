#include "huffman.h"

#include <tuple>

#include "binary_trie.h"
#include "priority_queue.h"

bool PointerCompare(const BinaryTrie::Pointer& lhs, const BinaryTrie::Pointer& rhs) {
    return *lhs > *rhs;
}

CodeSizes HuffmanEncoding(const SymbolsCount& symbols_count) {
    PriorityQueue<BinaryTrie::Pointer, decltype(PointerCompare)*> queue(PointerCompare);
    queue.Reserve(symbols_count.size());

    for (const auto& [key, count] : symbols_count) {
        queue.Push(std::make_shared<BinaryTrie>(key, count));
    }

    while (queue.Size() > 1) {
        BinaryTrie::Pointer left = queue.Top();
        queue.Pop();
        BinaryTrie::Pointer right = queue.Top();
        queue.Pop();
        BinaryTrie::Pointer node = std::make_shared<BinaryTrie>(left, right);
        queue.Push(node);
    }
    BinaryTrie::Pointer root = queue.Top();
    queue.Pop();

    CodeSizes sizes;
    sizes.reserve(symbols_count.size());
    auto callback = [&](std::size_t, std::size_t size, Char key) { sizes.push_back(CodeSize{key, size}); };
    root->Traverse(callback);
    std::sort(sizes.begin(), sizes.end());

    return sizes;
};

CodeTable CanonicalCodes(const CodeSizes& sizes) {
    CodeTable codes;

    std::size_t current_code = 0;
    std::size_t current_size = 1;
    for (const auto& [key, size] : sizes) {
        while (current_size < size) {
            current_code <<= 1;
            ++current_size;
        }
        codes[key] = Code{current_code, current_size};
        ++current_code;
    }

    return codes;
}

bool CodeSize::operator<(const CodeSize& other) const {
    return std::tie(size, key) < std::tie(other.size, other.key);
}
