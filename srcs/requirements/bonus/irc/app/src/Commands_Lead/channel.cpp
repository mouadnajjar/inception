#include "channel.hpp"


Channel::Channel(const std::string& name) 
    : _name(name), _topic(""), _password(""), 
      _isInviteOnly(false), _isTopicRestricted(false), 
      _hasPassword(false), _hasLimit(false), _userLimit(0) 
{

}

Channel::~Channel() {}


void Channel::addClient(Client* client) 
{
    if (!isClientInChannel(client))
        _clients.push_back(client);
}

void Channel::removeClient(Client *client)
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (*it == client)
        {
            _clients.erase(it);
            break;
        }   
    }
}

void Channel::addOperator(Client* client) 
{
    if (!isOperator(client)) {
        _operators.push_back(client);
    }
}

void Channel::removeOperator(Client* client) 
{
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
        if (*it == client) {
            _operators.erase(it);
            break;
        }
    }
}

void Channel::inviteClient(Client* client) 
{
    if (!isInvited(client)) {
        _invitedList.push_back(client);
    }
}

bool Channel::isClientInChannel(Client* client) const 
{
    return (std::find(_clients.begin(), _clients.end(), client) != _clients.end());
}

bool Channel::isOperator(Client* client) const 
{
    return (std::find(_operators.begin(), _operators.end(), client) != _operators.end());
}

bool Channel::isInvited(Client* client) const 
{
    return (std::find(_invitedList.begin(), _invitedList.end(), client) != _invitedList.end());
}