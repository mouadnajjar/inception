#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"

void Server::handleKick(Client &client, const Message &msg)
{
    if (!client.isRegistered())
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered\r\n");
    if (msg.params.empty() || msg.params.size() < 2)
        throw CommandExceptions(ERR_NEEDMOREPARAMS, "KICK :Not enough parameters\r\n");

    std::string channelName = msg.params[0];
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
        throw CommandExceptions(ERR_NOSUCHCHANNEL, channelName + " :No such channel\r\n");

    Channel &UserChannelSearch = it->second;
    if (!UserChannelSearch.isClientInChannel(&client))
        throw CommandExceptions(ERR_NOTONCHANNEL, channelName + " :You're not on that channel\r\n");

    if (!UserChannelSearch.isOperator(&client))
        throw CommandExceptions(ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator\r\n");

    std::string TargetUser = msg.params[1];
    Client* victimTarget = NULL; 
    std::vector<Client*> occupants = UserChannelSearch.getClients();
    for (size_t i = 0; i < occupants.size(); ++i)
    {
        if (occupants[i]->getNickname() == TargetUser)
        {
            victimTarget = occupants[i];
            break;
        }
    }
    if (victimTarget == NULL)
        throw CommandExceptions(ERR_USERNOTINCHANNEL, TargetUser + " " + channelName + " :They aren't on that channel\r\n");
    std::string kickComment = (msg.params.size() > 2) ? msg.params[2] : client.getNickname();
    std::string userPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    std::string kickMsg = userPrefix + " KICK " + channelName + " " + TargetUser + " :" + kickComment + "\r\n";
    for (size_t i = 0; i < occupants.size(); ++i)
    {
        occupants[i]->appendToWriteBuffer(kickMsg);
    }
    UserChannelSearch.removeClient(victimTarget);
    UserChannelSearch.removeOperator(victimTarget);
    
    if (UserChannelSearch.getClients().empty()) {
        _channels.erase(it);
    }
}