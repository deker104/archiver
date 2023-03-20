#ifndef ARCHIVER_CONSTANTS_
#define ARCHIVER_CONSTANTS_

#include <cstdint>

using Char = std::int16_t;

const Char FILENAME_END = 256;
const Char ONE_MORE_FILE = 257;
const Char END_OF_ARCHIVE = 258;

#endif  // ARCHIVER_CONSTANTS_