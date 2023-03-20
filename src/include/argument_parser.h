#ifndef ARCHIVER_ARGUMENT_PARSER_
#define ARCHIVER_ARGUMENT_PARSER_

#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "exceptions.h"

class ArgumentParser {
public:
    struct ParsedArguments {
        std::unordered_set<std::string> options;
        std::vector<std::string> positional_arguments;
    };

    struct OptionData {
        std::string description;
        std::string usage;

        OptionData();
        OptionData(std::string_view description, std::string_view usage);
    };

    explicit ArgumentParser(std::string_view program_name);

    void AddOption(std::string_view name, std::string_view description, std::string_view usage) noexcept;
    ParsedArguments ParseArguments(int argc, char** argv) const;
    void PrintUsage() const noexcept;

private:
    std::string program_name_;
    std::vector<std::string> ordered_options_;
    std::unordered_map<std::string, OptionData> options_;
};

#endif  // ARCHIVER_ARGUMENT_PARSER_
