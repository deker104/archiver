#include "binary_trie.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <tuple>

const BinaryTrie BinaryTrie::DEFAULT_VALUE = BinaryTrie();

BinaryTrie::BinaryTrie() = default;

BinaryTrie::BinaryTrie(Char key, std::size_t count) : count_(count), key_(key), terminal_(true) {
}

BinaryTrie::BinaryTrie(Pointer left, Pointer right) : left_(left), right_(right) {
    Relax();
}

std::strong_ordering BinaryTrie::operator<=>(const BinaryTrie& other) const {
    return std::tie(count_, key_) <=> std::tie(other.count_, other.key_);
}

bool BinaryTrie::IsTerminal() const {
    return terminal_;
}

bool BinaryTrie::IsLeaf() const {
    return left_ == nullptr;
}

void BinaryTrie::Relax() {
    key_ = std::min(CLeft().key_, CRight().key_);
    count_ = CLeft().count_ + CRight().count_;
}

BinaryTrie& BinaryTrie::Left() {
    if (left_ == nullptr) {
        left_ = std::make_shared<BinaryTrie>();
    }
    return *left_;
}

BinaryTrie& BinaryTrie::Right() {
    if (right_ == nullptr) {
        right_ = std::make_shared<BinaryTrie>();
    }
    return *right_;
}

const BinaryTrie& BinaryTrie::CLeft() const {
    if (left_ == nullptr) {
        return DEFAULT_VALUE;
    }
    return *left_;
}

const BinaryTrie& BinaryTrie::CRight() const {
    if (right_ == nullptr) {
        return DEFAULT_VALUE;
    }
    return *right_;
}

void BinaryTrie::Traverse(Callback callback, std::size_t code, std::size_t height) const {
    if (IsTerminal()) {
        callback(code, height, key_);
        return;
    }
    if (IsLeaf()) {
        return;
    }
    CLeft().Traverse(callback, code << 1, height + 1);
    CRight().Traverse(callback, (code << 1) | 1, height + 1);
}

void BinaryTrie::AddCode(std::size_t code, std::size_t size, Char key, std::size_t count, std::size_t height) {
    if (size == height) {
        if (!IsLeaf() || IsTerminal()) {
            throw CodeAlreadyExists();
        }
        key_ = key;
        count_ = count;
        terminal_ = true;
        return;
    }
    std::size_t bit_position = size - height - 1;
    if ((code >> bit_position) & 1) {
        Right().AddCode(code, size, key, count, height + 1);
    } else {
        Left().AddCode(code, size, key, count, height + 1);
    }
    Relax();
}
