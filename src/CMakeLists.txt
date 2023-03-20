include_directories(include)

add_executable(
        archiver
        archiver.cpp
        argument_parser.cpp
        binary_trie.cpp
        bit_stream.cpp
        compressor.cpp
        decompressor.cpp
        files.cpp
        huffman.cpp
)

add_catch(
        unittest
        test.cpp
        argument_parser.cpp
        binary_trie.cpp
        bit_stream.cpp
)
