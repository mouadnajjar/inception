#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"

void Server::handleInvite(Client &client, const Message &msg)
{
    if (!client.isRegistered()) {
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered\r\n");
    }

    if (msg.params.empty() || msg.params.size() < 2) {
        throw CommandExceptions(ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters\r\n");
    }

    Client *targetClient = NULL;
    std::string target = msg.params[0];
    std::string targetChannel = msg.params[1];

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == target)
        {
            targetClient = it->second;
            break;
        }
    }
    if(targetClient == NULL)
        throw CommandExceptions(ERR_NOSUCHNICK, target + " :No such nick/channel\r\n");
    
    std::map<std::string, Channel>::iterator chanIt = _channels.find(targetChannel);
    if (chanIt == _channels.end())
        throw CommandExceptions(ERR_NOSUCHCHANNEL, targetChannel + " :No such channel\r\n");

    Channel &targetCh = chanIt->second;

    if (!targetCh.isClientInChannel(&client))
        throw CommandExceptions(ERR_NOTONCHANNEL, targetChannel + " :You're not on that channel\r\n");
    
    if (targetCh.isClientInChannel(targetClient))
        throw CommandExceptions(ERR_USERONCHANNEL, target + " " + targetChannel + " :is already on channel\r\n");
    if (targetCh.getInviteOnly() && !targetCh.isOperator(&client))
        throw CommandExceptions(ERR_CHANOPRIVSNEEDED, targetChannel + " :You're not channel operator\r\n");

    targetCh.inviteClient(targetClient);
    sendNumericReply(client, RPL_INVITING, targetChannel + " " + target + "\r\n");

    std::string userPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();

    std::string inviteMsg = userPrefix + " INVITE " + target + " :" + targetChannel + "\r\n";
    targetClient->appendToWriteBuffer(inviteMsg);
}
