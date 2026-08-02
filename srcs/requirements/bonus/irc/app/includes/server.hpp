#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <fcntl.h>
#include <csignal>
#include <arpa/inet.h>
#include <cstdlib>
#include "Message.hpp"
#include "Exceptions.hpp"
#include "channel.hpp"
#include "client.hpp"


class Client;

class Server
{
    private:
         int _serverSocket;
         int _port;
         std::string _password;
         std::vector<pollfd> _pollFds; ///dynamic, expanding list of instruction cards for your server's multiplexer.
         std::map<int, Client*> _clients;                // Tracks all users
        std::map<std::string, Channel> _channels;

         void attemptRegistration(Client& client);
        //          helper func in runServer();
         void acceptNewClient();
         void receiveOldClientData(size_t &i);
         void processClientBuffer(int fd, const char* buffer);
         void sendClientData(size_t &i);
	 	void clearAllResources();
		void handleClientDisconnect(size_t &i);

        typedef void (Server::*CommandHandler)(Client &client, const Message &msg);
        std::map<std::string, CommandHandler> _commands;

        void sendNumericReply(Client& client, const std::string& code, const std::string& reply);

        void handlePass(Client& client, const Message& msg);
        void handleNick(Client& client, const Message& msg);
        void handleUser(Client& client, const Message& msg);
        void handleJoin(Client& client, const Message& msg);
        void handlePrivmsg(Client& client, const Message& msg);
        void handleKick(Client& client, const Message& msg);
        void handleInvite(Client& client, const Message& msg);
        void handleTopic(Client& client, const Message& msg);
        void handlePing(Client& client, const Message& msg);
        void handlePart(Client& client, const Message& msg);
        void handleMode(Client& client, const Message& msg);

        std::vector<std::string> splitString(const std::string& str, char delimiter);

        public:
        Server(int port, std::string password);
		~Server();

	    void initServer();
	    void runServer();

        void initCommands();
        void executeCommand(Client& client, const std::string& rawLine);
};


#endif
