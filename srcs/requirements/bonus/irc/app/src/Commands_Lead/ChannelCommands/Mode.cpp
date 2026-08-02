#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "Exceptions.hpp"
#include "NumericReplies.hpp"
#include <sstream>

void Server::handleMode(Client &client, const Message &msg)
{
    if (!client.isRegistered()){
        throw CommandExceptions(ERR_NOTREGISTERED, ":You have not registered\r\n");
    }
    if (msg.params.empty()){
        throw CommandExceptions(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters\r\n");
    }
    std::string channelName = msg.params[0];
    std::map<std::string, Channel>::iterator chanIt = _channels.find(channelName);
    if (chanIt == _channels.end())
    {
        throw CommandExceptions(ERR_NOSUCHCHANNEL, channelName + " :No such channel\r\n");
    }
    Channel &targetChannel = chanIt->second;
    if (msg.params.size() == 1)
    {
        std::string activeModes = "+";
        std::string modeArgs = "";

        if (targetChannel.getInviteOnly()) { 
            activeModes += "i";
        }
        if (targetChannel.getTopicRestricted()) { 
            activeModes += "t";
        }

        if (!targetChannel.getPassword().empty()) {
            activeModes += "k";
            modeArgs += " " + targetChannel.getPassword();
        }

        if (targetChannel.getHasLimit() > 0) {
            activeModes += "l";
            std::ostringstream ss;
            ss << targetChannel.getUserLimit();
            modeArgs += " " + ss.str();
        }
        std::string finalModeString = activeModes + modeArgs;
        std::string replyData = channelName + " " + finalModeString + "\r\n";
        sendNumericReply(client, RPL_CHANNELMODEIS, replyData);
        return;
    }
    if (msg.params.size() > 1)
    {
        if (!targetChannel.isOperator(&client)) {
            throw CommandExceptions(ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator\r\n");
        }

        std::string flags = msg.params[1];
        char currentSign = '+';
        size_t argIndex = 2;

        std::string appliedFlags = ""; 
        std::string appliedArgs = "";

        for (size_t i = 0; i < flags.size(); i++)
        {
            char c = flags[i];

            if (c == '+' || c == '-') {
                currentSign = c;
                continue;
            }

            switch (c) {
                case 'i':
                        if (currentSign == '+') {
                            targetChannel.setInviteOnly(true);
                        } else {
                            targetChannel.setInviteOnly(false);
                        }
                        appliedFlags += currentSign;
                        appliedFlags += 'i';
                        break;
                
                case 't':
                        if (currentSign == '+') {
                            targetChannel.setTopicRestricted(true);
                        } else {
                            targetChannel.setTopicRestricted(false);
                        }
                        appliedFlags += currentSign;
                        appliedFlags += 't';
                        break;
                
                case 'k':
                        if (currentSign == '+') {
                            if (argIndex < msg.params.size()) {
                                targetChannel.setPassword(msg.params[argIndex]);
                                appliedFlags += "+k";
                                appliedArgs += " " + msg.params[argIndex];
                                argIndex++;
                            }
                        } else {
                            targetChannel.removePassword();
                            appliedFlags += "-k";
                        }
                        break;
                case 'l':
                        if (currentSign == '+') {
                            if (argIndex < msg.params.size()) {
                                int limit = std::atoi(msg.params[argIndex].c_str());
                                if (limit > 0) {
                                    targetChannel.setUserLimit(static_cast<size_t>(limit));
                                    appliedFlags += "+l";
                                    appliedArgs += " " + msg.params[argIndex];
                                }
                                argIndex++;
                            }
                        } else {
                            targetChannel.removeUserLimit();
                            appliedFlags += "-l";
                        }
                        break;
                
                case 'o':
                        if (argIndex < msg.params.size()) {
                            std::string targetNick = msg.params[argIndex];
                            Client* targetClient = NULL;
                            for (size_t j = 0; j < targetChannel.getClients().size(); j++) {
                                if (targetChannel.getClients()[j]->getNickname() == targetNick) {
                                    targetClient = targetChannel.getClients()[j];
                                    break;
                                }
                            }
                            if (targetClient != NULL) {
                                if (currentSign == '+') {
                                    targetChannel.addOperator(targetClient);
                                    appliedFlags += "+o";
                                } else {
                                    targetChannel.removeOperator(targetClient);
                                    appliedFlags += "-o";
                                }
                                appliedArgs += " " + targetNick;
                            } else {
                                std::string errMsg = targetNick + " " + channelName + " :They aren't on that channel\r\n";
                                sendNumericReply(client, ERR_USERNOTINCHANNEL, errMsg);
                            }
                            argIndex++;
                        }
                            break;
                
                default:
                        std::string errMsg = std::string(1, c) + " :is unknown mode char to me\r\n";
                        sendNumericReply(client, ERR_UNKNOWNMODE, errMsg);
                        break;
            }
        }

        if (!appliedFlags.empty()) {
            std::string modeMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " MODE " + channelName + " " + appliedFlags + appliedArgs + "\r\n";
            std::vector<Client*>& channelClients = targetChannel.getClients();
            for (size_t i = 0; i < channelClients.size(); i++) {
                std::string currentReply = modeMessage;
                channelClients[i]->appendToWriteBuffer(currentReply);
            }
        }
    }
}