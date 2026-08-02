#include "server.hpp"
#include "parser.hpp"
#include "NumericReplies.hpp"
#include <iostream>

void Server::initCommands()
{
    _commands["PASS"] = &Server::handlePass;
    _commands["NICK"] = &Server::handleNick;
    _commands["USER"] = &Server::handleUser;
    _commands["JOIN"] = &Server::handleJoin;
    _commands["PRIVMSG"] = &Server::handlePrivmsg;
    _commands["KICK"] = &Server::handleKick;
    _commands["INVITE"] = &Server::handleInvite;
    _commands["TOPIC"] = &Server::handleTopic;
    _commands["PING"] = &Server::handlePing;
    _commands["PART"] = &Server::handlePart;
    _commands["MODE"] = &Server::handleMode;
}


void Server::executeCommand(Client &client, const std::string &Rawline)
{
  Message msg = Parser::parseRawString(Rawline);
  
    if (!msg.isValid || msg.command.empty())
        return ;

    std::map<std::string, CommandHandler> ::iterator it = _commands.find(msg.command);

    if (it != _commands.end())
    {
        try
        {
            ((this->*(it->second))(client, msg));
        }
        catch (const CommandExceptions &e)
        {
            sendNumericReply(client, e.getCode(), e.what());
        }
        catch (const std::exception& e) {
            std::cerr << "Fatal error: " << e.what() << std::endl;
        }
    }
    else 
    {
        sendNumericReply(client, ERR_UNKNOWNCOMMAND, msg.command + " :Unknown command\r\n");
    }
}