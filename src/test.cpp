#include <catch.hpp>
#include <cstring>
#include <initializer_list>
#include <string_view>
#include <sstream>

#include "argument_parser.h"
#include "binary_trie.h"
#include "bit_stream.h"
#include "priority_queue.h"

std::pair<int, char**> GenerateArgv(std::initializer_list<std::string> args) {
    int argc = static_cast<int>(args.size());
    char** argv = new char*[argc + 1];
    auto current = argv;
    for (auto argument : args) {
        *current = new char[argument.size() + 1];
        std::memcpy(*current, argument.data(), argument.size());
        (*current)[argument.size()] = 0;
        ++current;
    }
    *current = nullptr;
    return {argc, argv};
}

void ClearArgv(int argc, char** argv) {
    auto current = argv;
    for (int i = 0; i < argc; ++i) {
        delete[] current[i];
    }
    delete[] current;
}

TEST_CASE("ArgumentParser") {
    ArgumentParser argument_parser("test");
    argument_parser.AddOption("-h", "help", "-h");
    argument_parser.AddOption("-h", "help", "-h");
    argument_parser.PrintUsage();
    {
        auto [argc, argv] = GenerateArgv({"test", "-h", "hello"});
        argument_parser.ParseArguments(argc, argv);
        ClearArgv(argc, argv);
    }
    {
        auto [argc, argv] = GenerateArgv({"test", "-h", "-h"});
        try {
            argument_parser.ParseArguments(argc, argv);
            REQUIRE(false);
        } catch (const ParsingError& ex) {
        }
        ClearArgv(argc, argv);
    }
    {
        auto [argc, argv] = GenerateArgv({"test", "-c"});
        try {
            argument_parser.ParseArguments(argc, argv);
            REQUIRE(false);
        } catch (const ParsingError& ex) {
        }
        ClearArgv(argc, argv);
    }
    {
        auto [argc, argv] = GenerateArgv({"test", "hello", "-h"});
        try {
            argument_parser.ParseArguments(argc, argv);
            REQUIRE(false);
        } catch (const ParsingError& ex) {
        }
        ClearArgv(argc, argv);
    }
}

TEST_CASE("BitReader") {
    {
        std::stringstream ss;
        BitReader reader(ss);
        ss.put(0b00101011);
        ss.put(0b11110000);
        REQUIRE(reader.ReadBits<int>(8) == 0b00101011);
        REQUIRE(reader.ReadBit() == 1);
        REQUIRE(reader.ReadBit() == 1);
        REQUIRE(reader.ReadBit() == 1);
        REQUIRE(reader.ReadBit() == 1);
        REQUIRE(reader.ReadBit() == 0);
        REQUIRE(reader.ReadBit() == 0);
        REQUIRE(reader.ReadBit() == 0);
        REQUIRE(reader.ReadBit() == 0);
        try {
            reader.ReadBit();
            REQUIRE(false);
        } catch (const BitReader::EndOfFile& ex) {
        }
    }
    {
        std::stringstream ss;
        BitReader reader(ss);
        for (std::size_t i = 0; i < 2500; ++i) {
            ss.put(0b00101011);
            ss.put(0b11110000);
        }
        for (std::size_t i = 0; i < 2500; ++i) {
            reader.ReadBits<int>(16);
        }
    }
}

TEST_CASE("BitWriter") {
    {
        std::stringstream ss;
        {
            BitWriter writer(ss);
            writer.WriteBits<int>(0b00101011, 8);
            writer.WriteBit(true);
            writer.WriteBit(true);
            writer.WriteBit(true);
            writer.WriteBit(true);
            writer.WriteBit(false);
            writer.WriteBit(false);
            writer.WriteBit(false);
            writer.WriteBit(false);
        }
        REQUIRE(ss.get() == 0b00101011);
        REQUIRE(ss.get() == 0b11110000);
    }
    {
        std::stringstream ss;
        {
            BitWriter writer(ss);
            for (std::size_t i = 0; i < 2500; ++i) {
                writer.WriteBits<int>(0b00101011, 8);
                writer.WriteBits<int>(0b11110000, 8);
            }
        }
        for (std::size_t i = 0; i < 2500; ++i) {
            ss.get();
            ss.get();
        }
    }
}

TEST_CASE("BinaryTrie") {
    {
        BinaryTrie::Pointer a = std::make_shared<BinaryTrie>('a', 1);
        BinaryTrie::Pointer b = std::make_shared<BinaryTrie>('b', 2);
        REQUIRE(*a < *b);
        BinaryTrie::Pointer ab = std::make_shared<BinaryTrie>(a, b);
        REQUIRE(ab->count_ == 3);
        REQUIRE(ab->key_ == 'a');
        BinaryTrie::Pointer c = std::make_shared<BinaryTrie>('c', 2);
        BinaryTrie::Pointer abc = std::make_shared<BinaryTrie>(ab, c);
        REQUIRE(abc->count_ == 5);
        REQUIRE(abc->key_ == 'a');
        std::vector<char> visited;
        auto callback = [&](std::size_t code, std::size_t height, Char key) { visited.push_back(key); };
        abc->Traverse(callback);
        REQUIRE(visited == std::vector{'a', 'b', 'c'});
    }
    {
        BinaryTrie::Pointer trie = std::make_shared<BinaryTrie>();
        trie->AddCode(0b1, 1, 'c');
        trie->AddCode(0b000, 3, 'a');
        trie->AddCode(0b001, 3, 'b');
        try {
            trie->AddCode(0b1, 1, 'c');
            REQUIRE(false);
        } catch (const BinaryTrie::CodeAlreadyExists& ex) {
        }
        try {
            trie->AddCode(0b00, 2, 'd');
            REQUIRE(false);
        } catch (const BinaryTrie::CodeAlreadyExists& ex) {
        }
        std::vector<char> visited;
        auto callback = [&](std::size_t code, std::size_t height, Char key) { visited.push_back(key); };
        trie->Traverse(callback);
        REQUIRE(visited == std::vector{'a', 'b', 'c'});
    }
}

TEST_CASE("PriorityQueue") {
    using PQ = PriorityQueue<int>;
    {
        PQ queue;
        try {
            queue.Top();
            REQUIRE(false);
        } catch (const PQ::QueueIsEmpty& ex) {
        }
        queue.Pop();
    }
    {
        PQ queue{5, 2, 3, 1, 4};
        for (std::size_t i = 5; i >= 1; --i) {
            REQUIRE(queue.Top() == i);
            queue.Pop();
        }
    }
}
