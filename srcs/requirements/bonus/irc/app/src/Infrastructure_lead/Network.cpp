#include "server.hpp"
#include <iostream>

void Client::setHostname(const std::string& ip) {
	_hostname = ip;
}

std::string Client::getHostname() const {
	return _hostname;
}

void Server::acceptNewClient()
{
	sockaddr_in clientAddress;
	socklen_t clientLen = sizeof(clientAddress);

	int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
	if (clientSocket == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		std::cerr << "accept: " << strerror(errno) << std::endl;
		return;
	}

	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: Could not make client non-blocking!" << std::endl;
		close(clientSocket);
		return;
	}

	std::string ipAddress = inet_ntoa(clientAddress.sin_addr);

	pollfd newClientPoll;
	newClientPoll.fd = clientSocket;
	newClientPoll.events = POLLIN;
	newClientPoll.revents = 0;
	_pollFds.push_back(newClientPoll);

	Client* newClient = new Client(clientSocket);
	newClient->setHostname(ipAddress);
	_clients[clientSocket] = newClient;

	std::cout << "New client connected! Socket ID: " << clientSocket << " | IP: " << ipAddress << std::endl;
}

void Server::receiveOldClientData(size_t &i)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	int bytesReceived = recv(_pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);

	if (bytesReceived == 0)
	{
		std::cout << "Client " << _pollFds[i].fd << " connection dropped." << std::endl;
		handleClientDisconnect(i);
	}
	else if(bytesReceived < 0)
	{
		if( errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		std::cerr << "recv error on fd " << _pollFds[i].fd << ": " << strerror(errno) << std::endl;
		handleClientDisconnect(i);
	}
	else
	{
		processClientBuffer(_pollFds[i].fd, buffer);
	}
}

void Server::processClientBuffer(int fd, const char* buffer)
{
	Client* client = _clients[fd];
	client->appendToReadBuffer(buffer);

	std::string& readBuffer = client->getReadBuffer();

	size_t pos;
	while ((pos = readBuffer.find("\r\n")) != std::string::npos)
	{
		std::string command = readBuffer.substr(0, pos);
		readBuffer.erase(0, pos + 2);

		if (command.length() > 510)
		{
			std::cout << "[WARNING] A single command exceeded 512 bytes. Dropped." << std::endl;
			continue;
		}

		std::cout << "PERFECT COMMAND EXTRACTED: [" << command << "]" << std::endl;
		executeCommand(*client, command);
	}

	if (readBuffer.length() > 2048)
	{
		std::cout << "[WARNING] Client " << fd << " is flooding without \\r\\n. Wiping." << std::endl;
		readBuffer.clear();
	}
}

void Server::sendClientData(size_t &i)
{
	int fd = _pollFds[i].fd;
	Client* client = _clients[fd];
	std::string& writeBuffer = client->getWriteBuffer();

	if (writeBuffer.empty()) {
		_pollFds[i].events &= ~POLLOUT;
		return;
	}

	size_t total_sent = 0;
	size_t length = writeBuffer.length();
	int bytesSent = 0;

	while (total_sent < length)
	{
		bytesSent = send(fd, writeBuffer.c_str() + total_sent, length - total_sent, 0);

		if (bytesSent < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
			else {
				std::cerr << "[NETWORK] Send error on FD " << fd << ": " << strerror(errno) << std::endl;
				handleClientDisconnect(i);
				return;
			}
		}
		else if (bytesSent == 0) {
			break;
		}

		total_sent += bytesSent;
	}
	if (total_sent > 0)
	{
		writeBuffer.erase(0, total_sent);
		if (writeBuffer.empty())
			_pollFds[i].events &= ~POLLOUT;
	}
}

void Server::handleClientDisconnect(size_t &i)
{
	int deadFd = _pollFds[i].fd;
	
	Client* deadClient = _clients[deadFd];

	if (deadClient != NULL)
	{
		std::string quitMsg = ":" + deadClient->getNickname() + "!user@" + deadClient->getHostname() + " QUIT :Connection closed\r\n";

		std::map<std::string, Channel>::iterator it = _channels.begin();
		while (it != _channels.end())
		{
			Channel& currentChan = it->second;
			
			if (currentChan.isClientInChannel(deadClient)) 
			{
				std::vector<Client*>& users = currentChan.getClients();
				
				// Broadcast QUIT to everyone else in the room
				for (size_t j = 0; j < users.size(); j++) {
					if (users[j] != deadClient) {
						users[j]->appendToWriteBuffer(quitMsg);
					}
				}
				currentChan.removeClient(deadClient);
				currentChan.removeOperator(deadClient);
			}
			if (currentChan.getClients().empty()) {
				std::map<std::string, Channel>::iterator toErase = it;
				++it;
				_channels.erase(toErase);
			} else {
				++it;
			}
		}
	}
	delete _clients[deadFd];
	_clients.erase(deadFd);
	close(deadFd);

	_pollFds[i] = _pollFds.back();
	_pollFds.pop_back();

	i--;
}

