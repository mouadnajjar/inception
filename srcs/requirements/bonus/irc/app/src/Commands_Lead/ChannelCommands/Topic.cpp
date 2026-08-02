#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"

void Server::handleTopic(Client &client, const Message &msg)
{
    if (!client.isRegistered())
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered\r\n");
    if (msg.params.empty())
        throw CommandExceptions(ERR_NEEDMOREPARAMS ,"TOPIC :Not enough parameters\r\n");
    
    std::string channelName = msg.params[0];
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
        throw CommandExceptions(ERR_NOSUCHCHANNEL, channelName + " :No such channel\r\n");
    Channel &targetChannel = it->second;
    if (!targetChannel.isClientInChannel(&client))
        throw CommandExceptions(ERR_NOTONCHANNEL, channelName + " :You're not on that channel\r\n");
    if (msg.params.size() == 1)
    {
        if (targetChannel.getTopic().empty())
            sendNumericReply(client, RPL_NOTOPIC, channelName + " :No topic is set\r\n");
        else
            sendNumericReply(client, RPL_TOPIC, channelName + " :" + targetChannel.getTopic() + "\r\n");
        
        return;
    }
    if (targetChannel.getTopicRestricted() && !targetChannel.isOperator(&client))
    {
        throw CommandExceptions(ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator\r\n");
    }
    std::string newTopic = msg.params[1];
    targetChannel.setTopic(newTopic);

    std::string userPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
    std::string topicMsg = userPrefix + " TOPIC " + channelName + " :" + newTopic + "\r\n";

    std::vector<Client*> occupants = targetChannel.getClients();
    for (size_t i = 0; i < occupants.size(); ++i)
    {
        occupants[i]->appendToWriteBuffer(topicMsg);
    }

}