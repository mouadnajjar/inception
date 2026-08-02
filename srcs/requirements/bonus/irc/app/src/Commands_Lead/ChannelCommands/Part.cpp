#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"

void Server::handlePart(Client &client, const Message &msg)
{
    if (!client.isRegistered()) {
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered\r\n");
    }

    if (msg.params.empty()) {
        throw CommandExceptions(ERR_NEEDMOREPARAMS, "PART :Not enough parameters\r\n");
    }
    std::string channelName = msg.params[0];
    std::string reason = (msg.params.size() > 1) ? msg.params[1] : "Leaving";
    std::map<std::string, Channel>::iterator chanIt = _channels.find(channelName);

    if (chanIt == _channels.end()) {
        throw CommandExceptions(ERR_NOSUCHCHANNEL, channelName + " :No such channel\r\n");
    }

    Channel &targetChannel = chanIt->second;

    if (!targetChannel.isClientInChannel(&client)) {
        throw CommandExceptions(ERR_NOTONCHANNEL, channelName + " :You're not on that channel\r\n");
    }

    std::string userPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    std::string partMsg = userPrefix + " PART " + channelName + " :" + reason + "\r\n";

    std::vector<Client*> occupants = targetChannel.getClients(); 
    for (size_t i = 0; i < occupants.size(); ++i) {
        occupants[i]->appendToWriteBuffer(partMsg);
    }

    targetChannel.removeClient(&client);

    if (targetChannel.isOperator(&client)) {
        targetChannel.removeOperator(&client);
    }

    if (targetChannel.getClients().empty()) {
        _channels.erase(chanIt);
    }
}