#ifndef ARCHIVER_PRIORITY_QUEUE_
#define ARCHIVER_PRIORITY_QUEUE_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <vector>

#include "exceptions.h"

template <typename T, typename Comparator = std::less<T>, typename Container = std::vector<T>>
class PriorityQueue {
public:
    class QueueIsEmpty : public std::exception {};

    explicit PriorityQueue(const Comparator& cmp = Comparator()) : cmp_(cmp) {
    }

    template <typename It>
    PriorityQueue(It begin, It end, const Comparator& cmp = Comparator()) : cmp_(cmp) {
        for (auto it = begin; it != end; ++it) {
            Push(*it);
        }
    }

    PriorityQueue(std::initializer_list<T> data, const Comparator& cmp = Comparator())
        : PriorityQueue(data.begin(), data.end(), cmp) {
    }

    const T& Top() const {
        if (IsEmpty()) {
            throw QueueIsEmpty();
        }
        return heap_[0];
    }

    void Pop() {
        if (IsEmpty()) {
            return;
        }
        std::swap(heap_[0], heap_[Size() - 1]);
        heap_.pop_back();
        HeapifyDown(0);
    }

    void Push(const T& value) {
        heap_.push_back(value);
        HeapifyUp(Size() - 1);
    }

    std::size_t Size() const {
        return heap_.size();
    }
    bool IsEmpty() const {
        return Size() == 0;
    }
    void Reserve(std::size_t size) {
        heap_.reserve(size);
    }

private:
    Container heap_;
    Comparator cmp_;

    static std::size_t LeftChild(std::size_t node) {
        return 2 * node + 1;
    }

    static std::size_t RightChild(std::size_t node) {
        return 2 * node + 2;
    }

    static std::size_t Parent(std::size_t node) {
        return (node - 1) / 2;
    }

    void HeapifyDown(std::size_t node) {
        std::size_t left = LeftChild(node);
        std::size_t right = RightChild(node);
        std::size_t max = node;

        if (left < Size() && cmp_(heap_[max], heap_[left])) {
            max = left;
        }
        if (right < Size() && cmp_(heap_[max], heap_[right])) {
            max = right;
        }
        if (max != node) {
            std::swap(heap_[max], heap_[node]);
            HeapifyDown(max);
        }
    }

    void HeapifyUp(std::size_t node) {
        if (node == 0) {
            return;
        }
        std::size_t parent = Parent(node);
        if (cmp_(heap_[parent], heap_[node])) {
            std::swap(heap_[parent], heap_[node]);
            HeapifyUp(parent);
        }
    }
};

#endif  // ARCHIVER_PRIORITY_QUEUE_
