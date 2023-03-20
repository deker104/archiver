#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string_view>

#include "argument_parser.h"
#include "compressor.h"
#include "decompressor.h"
#include "exceptions.h"
#include "files.h"

int main(int argc, char** argv) {
    ArgumentParser parser("archiver");
    parser.AddOption("-c", "compress files into archive", "-c archive_name file1 [file2 ...]");
    parser.AddOption("-d", "decompress archive", "-d archive_name");
    parser.AddOption("-h", "show this message", "-h");

    try {
        auto parsed_arguments = parser.ParseArguments(argc, argv);

        if (parsed_arguments.options.size() > 1) {
            throw ValidationError("Too many options");
        } else if (parsed_arguments.options.empty()) {
            throw ValidationError("You need to specify at least one option");
        } else if (parsed_arguments.options.contains("-h")) {
            parser.PrintUsage();
            return 0;
        } else if (parsed_arguments.options.contains("-c")) {
            if (parsed_arguments.positional_arguments.size() < 2) {
                throw ValidationError("You need to specify archive name and at least one input file");
            }
            Path archive_name = parsed_arguments.positional_arguments[0];
            if (!ValidateOutput(archive_name)) {
                throw ValidationError("Archive destination is not valid");
            }
            std::vector<Path> filenames;
            for (std::size_t i = 1; i < parsed_arguments.positional_arguments.size(); ++i) {
                Path filename = parsed_arguments.positional_arguments[i];
                if (!ValidateInput(filename)) {
                    throw ValidationError("At least one of input files is not valid");
                }
                filenames.push_back(filename);
            }
            Compress(archive_name, filenames);
        } else if (parsed_arguments.options.contains("-d")) {
            if (parsed_arguments.positional_arguments.empty()) {
                throw ValidationError("You need to specify archive name");
            } else if (parsed_arguments.positional_arguments.size() > 1) {
                throw ValidationError("Too many positional arguments");
            }
            Path archive_name = parsed_arguments.positional_arguments[0];
            if (!ValidateInput(archive_name)) {
                throw ValidationError("Invalid archive path");
            }
            Decompress(archive_name);
        }
    } catch (const ParsingError& exc) {
        std::cerr << "ERROR: " << exc.what() << "\n\n";
        parser.PrintUsage();
        return 111;
    } catch (const ArchiverException& exc) {
        std::cerr << "ERROR: " << exc.what() << "\n\n";
        return 111;
    }

    return 0;
}