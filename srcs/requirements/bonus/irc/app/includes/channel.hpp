#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include "client.hpp"

class Channel
{
    private:
        std::string             _name;          // e.g., "#general"
        std::string             _topic;         // The room's topic
        std::string             _password;      // Used for mode +k

        std::vector<Client*>    _clients;       // Everyone currently in the room
        std::vector<Client*>    _operators;     // People with Operator (@) privileges
        std::vector<Client*>    _invitedList;   // People who have been invited (for +i)

        bool                    _isInviteOnly;      // Mode +i
        bool                    _isTopicRestricted; // Mode +t (Only Ops can change topic)
        bool                    _hasPassword;       // Mode +k
        bool                    _hasLimit;          // Mode +l
        size_t                  _userLimit;         // The limit number for +l

    public:
        // Constructor & Destructor
        Channel(const std::string& name);
        ~Channel();

        // --- GETTERS ---
        const std::string& getName() const { return _name; }
        const std::string& getTopic() const { return _topic; }
        const std::string& getPassword() const { return _password; }
        bool getInviteOnly() const { return _isInviteOnly; }
        bool getTopicRestricted() const { return _isTopicRestricted; }
        bool getHasLimit() const { return _hasLimit; }
        size_t getUserLimit() const { return _userLimit; }
        size_t getClientCount() const { return _clients.size(); }
        std::vector<Client*>& getClients() { return _clients; }

        // --- SETTERS ---
        void setTopic(const std::string& topic) { _topic = topic; }
        void setPassword(const std::string& password) { _password = password; _hasPassword = true; }
        void removePassword() { _password = ""; _hasPassword = false; }
        void setInviteOnly(bool status) { _isInviteOnly = status; }
        void setTopicRestricted(bool status) { _isTopicRestricted = status; }
        void setUserLimit(size_t limit) { _userLimit = limit; _hasLimit = true; }
        void removeUserLimit() { _userLimit = 0; _hasLimit = false; }

        // --- CORE FUNCTIONALITY ---
        void addClient(Client* client);
        void removeClient(Client* client);
        
        void addOperator(Client* client);
        void removeOperator(Client* client);
        
        void inviteClient(Client* client);
        
        // --- VERIFICATION HELPERS ---
        bool isClientInChannel(Client* client) const;
        bool isOperator(Client* client) const;
        bool isInvited(Client* client) const;
};

#endif