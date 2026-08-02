#include "server.hpp"
#include "client.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"
#include "iostream"

void Server::attemptRegistration(Client& client)
{
    if (client.isRegistered()) 
        return;

    if (client.isAuthenticated() && !client.getNickname().empty() && !client.getUsername().empty()) 
    {
        client.setRegistered(true);
        std::string welcomeMsg = ":Welcome to the Internet Relay Network " + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + "\r\n";
        
        sendNumericReply(client, RPL_WELCOME, welcomeMsg);
        
        std::cout << "Client " << client.getFd() << " is fully registered!" << std::endl;
    }
}

void Server::handlePing(Client &client, const Message &msg)
{
    if (msg.params.empty() || msg.params[0].empty()) {
        throw CommandExceptions(ERR_NOORIGIN, ":No origin specified\r\n");
    }

    std::string token = msg.params[0];

    std::string pongMsg = "PONG ft_irc :" + token + "\r\n";
    
    client.appendToWriteBuffer(pongMsg);
}

void Server::handlePass(Client &client, const Message &msg)
{
    if (client.isAuthenticated())
    {

        return ;
    }
    if (client.isRegistered())
    {
        throw CommandExceptions(ERR_ALREADYREGISTRED, ":Unauthorized command (already registered)\r\n");
    }
    if (msg.params.empty())
    {
        throw CommandExceptions(ERR_NEEDMOREPARAMS, "PASS :Not enough parameters\r\n");
    }
    if (msg.params[0] == this->_password)
    {
        client.setAuthenticated(true);
        attemptRegistration(client);
    } else {
        client.setAuthenticated(false);
        throw CommandExceptions(ERR_PASSWDMISMATCH, ":Password incorrect\r\n");
    }
}

void Server::handleNick(Client &client, const Message &msg)
{
    if (!client.isAuthenticated()) {
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered (Send PASS first)\r\n");
    }
    if (msg.params.empty() || msg.params[0].empty())
    {
        throw CommandExceptions(ERR_NONICKNAMEGIVEN, ":No nickname given\r\n");
    }
    std::string newNick = msg.params[0];
    if (newNick.find_first_of(" ,*?!@#$:") != std::string::npos)
    {
        throw CommandExceptions(ERR_ERRONEUSNICKNAME, newNick + " :Erroneous nickname\r\n");
    }
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == newNick && it->second->getFd() != client.getFd())
        {
            throw CommandExceptions(ERR_NICKNAMEINUSE, newNick + " :Nickname is already in use\r\n");
        }
    }
    client.setNickname(newNick);
    attemptRegistration(client);
}

void Server::handleUser(Client &client, const Message &msg)
{
    if (!client.isAuthenticated()) {
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered (Send PASS first)\r\n");
    }
    if (client.isRegistered())
    {
        throw CommandExceptions(ERR_ALREADYREGISTRED, ":Unauthorized command (already registered)\r\n");
    }
    if (msg.params.size() < 4)
    {
        throw CommandExceptions(ERR_NEEDMOREPARAMS, "USER :Not enough parameters\r\n");
    }
    client.setUsername(msg.params[0]);
    client.setRealname(msg.params[3]);
    attemptRegistration(client);
}

void Server::sendNumericReply(Client& client, const std::string& code, const std::string& reply)
{
    std::string nick = client.getNickname();
    if (nick.empty())
    {
        nick = "*";
    }
    std::string fullreply = ":ft_irc " + code + " " + nick + " " + reply;
    client.appendToWriteBuffer(fullreply);
    std::cout << "-> Sent to Client " << client.getFd() << ": " << fullreply;
}