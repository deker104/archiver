#ifndef ARCHIVER_BINARY_TRIE_
#define ARCHIVER_BINARY_TRIE_

#include <compare>
#include <cstddef>
#include <exception>
#include <functional>
#include <limits>
#include <memory>

#include "constants.h"

class BinaryTrie {
    const static BinaryTrie DEFAULT_VALUE;

public:
    class CodeAlreadyExists : public std::exception {};

    using Callback = std::function<void(std::size_t, std::size_t, Char)>;
    using Pointer = std::shared_ptr<BinaryTrie>;

    BinaryTrie();
    BinaryTrie(Char key, std::size_t count);
    BinaryTrie(Pointer left, Pointer right);

    std::strong_ordering operator<=>(const BinaryTrie& other) const;

    bool IsTerminal() const;
    bool IsLeaf() const;

    void Traverse(Callback callback, std::size_t code = 0, std::size_t height = 0) const;
    void AddCode(std::size_t code, std::size_t size, Char key, std::size_t count = 1, std::size_t height = 0);

    BinaryTrie& Left();
    BinaryTrie& Right();

    const BinaryTrie& CLeft() const;
    const BinaryTrie& CRight() const;

    std::size_t count_ = 0;
    Char key_ = std::numeric_limits<Char>::max();

private:
    bool terminal_ = false;

    Pointer left_ = nullptr;
    Pointer right_ = nullptr;

    void Relax();
};

bool PointerCompare(const BinaryTrie::Pointer& lhs, const BinaryTrie::Pointer& rhs);

#endif  // ARCHIVER_BINARY_TRIE_
