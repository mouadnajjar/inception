#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"

void Server::handlePrivmsg(Client &client, const Message &msg)
{
    if (!client.isRegistered())
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered\r\n");
    if (msg.params.empty())
        throw CommandExceptions(ERR_NORECIPIENT, ":No recipient given (PRIVMSG)\r\n");
    if (msg.params.size() < 2 || msg.params[1].empty())
        throw CommandExceptions(ERR_NOTEXTTOSEND, ":No text to send\r\n");
    
    std::string target = msg.params[0];
    std::string text = msg.params[1];
    std::string userPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    if (target[0] == '#' || target[0] == '&')
    {
        std::map<std::string, Channel>::iterator it = _channels.find(target);
        if (it == _channels.end())
            throw CommandExceptions(ERR_NOSUCHNICK, " :No such nick/channel\r\n");
        if (!it->second.isClientInChannel(&client))
            throw CommandExceptions(ERR_CANNOTSENDTOCHAN, " :Cannot send to channel\r\n");
        
        Channel& targetChannel = it->second;

        std::string privmsg = userPrefix + " PRIVMSG " + target + " :" + text +"\r\n";
        std::vector<Client*> occupants = targetChannel.getClients();

        for (size_t i = 0; i < occupants.size(); i++)
        {
            if (occupants[i] != &client)
            {
                occupants[i]->appendToWriteBuffer(privmsg);
            }
        }
    }
    else 
    {
        bool targetFound = false;

        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if (it->second->getNickname() == target)
            {
                std::string privmsg = userPrefix + " PRIVMSG " + target + " :" + text + "\r\n";
                it->second->appendToWriteBuffer(privmsg);
                targetFound = true;
                break ;
            }
        }
        if (!targetFound){
            throw CommandExceptions(ERR_NOSUCHNICK, target + " :No such nick/channel\r\n");
        }
    }
}