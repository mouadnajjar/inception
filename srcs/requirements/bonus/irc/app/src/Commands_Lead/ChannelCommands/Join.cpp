#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"

void Server::handleJoin(Client &client, const Message &msg)
{
    if (!client.isRegistered()) {
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered\r\n");
    }

    if (msg.params.empty()) {
        throw CommandExceptions(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters\r\n");
    }

    std::vector<std::string> channels = splitString(msg.params[0], ',');
    std::vector<std::string> keys;
    if (msg.params.size() > 1) {
        keys = splitString(msg.params[1], ',');
    }

    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::string channelName = channels[i];    
        std::string password = (i < keys.size()) ? keys[i] : "";

        if (channelName.empty()) {
            continue; 
        }
        if (channelName[0] != '#' && channelName[0] != '&') {
            sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel\r\n");
            continue;
        }

        std::map<std::string, Channel>::iterator it = _channels.find(channelName);
        if (it == _channels.end())
        {
            _channels.insert(std::make_pair(channelName, Channel(channelName)));
            it = _channels.find(channelName);
            it->second.addOperator(&client);
            
            if (!password.empty()) { it->second.setPassword(password); }
        } 
        else
        {
            Channel& chan = it->second;
            
            if (chan.isClientInChannel(&client)) {
                continue;
            }
            if (chan.getHasLimit() && chan.getClientCount() >= chan.getUserLimit()) {
                sendNumericReply(client, ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)\r\n");
                continue;
            }
            // Mode +k (Password/Key)
            if (chan.getPassword() != "" && chan.getPassword() != password) {
                sendNumericReply(client, ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)\r\n");
                continue;
            }
            // Mode +i (Invite Only)
            if (chan.getInviteOnly() && !chan.isInvited(&client)) {
                sendNumericReply(client, ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)\r\n");
                continue;
            }
        }

        Channel& targetChannel = it->second;
        targetChannel.addClient(&client);

        std::string userPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
        std::string joinMsg = userPrefix + " JOIN :" + channelName + "\r\n";

        // Broadcast to everyone in the channel
        std::vector<Client*> occupants = targetChannel.getClients();
        for (size_t j = 0; j < occupants.size(); ++j) {
            occupants[j]->appendToWriteBuffer(joinMsg);
        }

        // Send Topic
        if (targetChannel.getTopic().empty()) {
            sendNumericReply(client, RPL_NOTOPIC, channelName + " :No topic is set\r\n");
        } else {
            sendNumericReply(client, RPL_TOPIC, channelName + " :" + targetChannel.getTopic() + "\r\n");
        }

        // Send Names List
        std::string namesList = "= " + channelName + " :";
        for (size_t j = 0; j < occupants.size(); ++j) {
            if (targetChannel.isOperator(occupants[j])) {
                namesList += "@";
            }
            namesList += occupants[j]->getNickname() + " ";
        }
        namesList += "\r\n";
        
        sendNumericReply(client, RPL_NAMREPLY, namesList);
        sendNumericReply(client, RPL_ENDOFNAMES, channelName + " :End of /NAMES list\r\n");
    }
}