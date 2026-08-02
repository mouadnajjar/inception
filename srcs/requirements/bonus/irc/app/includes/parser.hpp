#ifndef PARSER_HPP
#define PARSER_HPP

#include "Message.hpp"
#include <string>

class Parser {
public:
    static Message parseRawString(std::string rawLine);
};

#endif