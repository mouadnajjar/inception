#include "Message.hpp"
#include "parser.hpp"
#include "server.hpp"

Message Parser::parseRawString(std::string rawLine)
{
    Message Result;

    if (rawLine.length() > MAX_MSG_LEN)
    {
        Result.isValid = false; 
        return Result;
    }
    ///r/n PART
    if (!rawLine.empty() && rawLine[rawLine.size() - 1] == '\n')
        rawLine.erase(rawLine.size() - 1, 1);
    if (!rawLine.empty() && rawLine[rawLine.size() - 1] == '\r')
        rawLine.erase(rawLine.size() - 1, 1);

    if (rawLine.empty()) return Result;


    size_t firstNotSpace = rawLine.find_first_not_of(' ');
    if (firstNotSpace == std::string::npos) return Result;
        rawLine = rawLine.substr(firstNotSpace);
    // PREFIX EXTARCTION
    if (rawLine[0] == ':')
    {
        size_t index = rawLine.find(' ');
        if (index != std::string::npos)
        {
            Result.prefix = rawLine.substr(0, index);
            rawLine.erase(0, index + 1);
        }
    }
    // Trailling param extraction
    size_t tpos = rawLine.find(" :");
    bool hasTrailing = false;
    std::string trailing;
    if (tpos != std::string::npos)
    {
        trailing = rawLine.substr(tpos + 2);
        rawLine.erase(tpos);
        hasTrailing = true;
    }

    std::stringstream ss(rawLine);
    ss >> Result.command;

    for (size_t i = 0; i < Result.command.size(); i++)
    {
        Result.command[i] = std::toupper(static_cast<unsigned char>(Result.command[i]));
    }

    std::string param;
    while (ss >> param)
    {
        if (Result.params.size() >= 14) {
            std::string remainder;
            std::getline(ss, remainder);
            if (!remainder.empty()) {
                // Remove leading spaces from remainder
                size_t firstChar = remainder.find_first_not_of(' ');
                if (firstChar!= std::string::npos)
                    param += remainder;
            }
            Result.params.push_back(param);
            break; 
        }
        Result.params.push_back(param);
    }
    if (hasTrailing && Result.params.size() < 15) {
        Result.params.push_back(trailing);
    }

    if (!Result.command.empty())
        Result.isValid = true;
    
    return Result;
}

std::vector<std::string> Server::splitString(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}