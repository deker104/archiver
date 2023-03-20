#include "argument_parser.h"

#include <iostream>
#include <string_view>

#include "exceptions.h"

ArgumentParser::OptionData::OptionData() = default;

ArgumentParser::OptionData::OptionData(std::string_view description, std::string_view usage)
    : description(description), usage(usage) {
}

ArgumentParser::ArgumentParser(std::string_view program_name) : program_name_(program_name) {
}

void ArgumentParser::AddOption(std::string_view name, std::string_view description, std::string_view usage) noexcept {
    if (options_.contains(name.data())) {
        return;
    }
    options_[name.data()] = OptionData(description, usage);
    ordered_options_.push_back(std::string(name));
}

ArgumentParser::ParsedArguments ArgumentParser::ParseArguments(int, char **argv) const {
    ParsedArguments parsed_arguments;
    ++argv;
    while (*argv) {
        std::string_view argument = *argv;
        if (argument.starts_with('-')) {
            if (!parsed_arguments.positional_arguments.empty()) {
                throw ParsingError("Found option after positional argument");
            }
            if (!options_.contains(argument.data())) {
                throw ParsingError("Options does not exist");
            }
            if (parsed_arguments.options.contains(argument.data())) {
                throw ParsingError("Option is specified multiple times");
            }
            parsed_arguments.options.insert(std::string(argument));
        } else {
            parsed_arguments.positional_arguments.push_back(std::string(argument));
        }
        ++argv;
    }
    return parsed_arguments;
}

void ArgumentParser::PrintUsage() const noexcept {
    std::cerr << "USAGE: ";
    std::cerr << program_name_ << ' ';
    std::cerr << "[-<option> ...] ";
    std::cerr << "[<argument> ...]\n\n";
    std::cerr << "OPTIONS:\n";
    for (const auto &name : ordered_options_) {
        const auto &[description, usage] = options_.at(name);
        std::cerr << "\t" << '`' << usage << '`' << "  --  " << description << '\n';
    }
}
