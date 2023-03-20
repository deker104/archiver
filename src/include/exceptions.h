#ifndef ARCHIVER_EXCEPTIONS_
#define ARCHIVER_EXCEPTIONS_

#include <exception>
#include <string>
#include <string_view>

class ArchiverException : public std::exception {
public:
    explicit ArchiverException(std::string_view message) : std::exception(), message_(message) {
    }

    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};

class ParsingError : public ArchiverException {
public:
    explicit ParsingError(std::string_view message) : ArchiverException(message) {
    }
};

class ValidationError : public ParsingError {
public:
    explicit ValidationError(std::string_view message) : ParsingError(message) {
    }
};

class InvalidFormat : public ArchiverException {
public:
    InvalidFormat() : ArchiverException("Invalid archive format") {
    }
};

class OutputError : public ArchiverException {
public:
    OutputError() : ArchiverException("Cannot write one of archived files") {
    }
};

#endif  // ARCHIVER_EXCEPTIONS_
