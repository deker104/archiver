#ifndef ARCHIVER_FILES_
#define ARCHIVER_FILES_

#include <filesystem>

using Path = std::filesystem::path;

bool ValidateOutput(Path archive_name);
bool ValidateInput(Path filename);

#endif  // ARCHIVER_FILES_