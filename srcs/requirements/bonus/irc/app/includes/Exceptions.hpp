#ifndef EXCEPTIONS_HPP

#define EXCEPTIONS_HPP

#include <iostream>
#include <exception>

class CommandExceptions : public std::exception 
{
    private:
        std::string _code;
        std::string _message;
    public:
        CommandExceptions(const std::string &code, const std::string &message) : _code(code), _message(message) {};
        virtual ~CommandExceptions() throw() {}
        virtual const char* what() const throw() {
            return _message.c_str();
        }
        const std::string& getCode() const {
            return _code;
        }
};

#endif