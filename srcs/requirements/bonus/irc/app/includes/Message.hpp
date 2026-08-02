#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>
#include <sstream>

#define MAX_MSG_LEN 512

struct Message {
    std::string prefix;            // Optional: starts with ':'
    std::string command;           // Mandatory: e.g., "NICK", "JOIN"
    std::vector<std::string> params; // Optional: up to 15 parameters
    bool isValid;                  // To track if parsing succeeded

    Message() : isValid(false) {}
};

#endif