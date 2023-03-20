#include "files.h"

bool ValidateOutput(Path archive_name) {
    return !std::filesystem::is_directory(archive_name);
}

bool ValidateInput(Path filename) {
    return std::filesystem::exists(filename) && !std::filesystem::is_directory(filename);
}
