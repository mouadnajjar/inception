#include "server.hpp"

#include <iostream>
extern volatile sig_atomic_t g_srvRunning;


Server::Server(int port, std::string password) : _serverSocket(-1) ,_port(port), _password(password)
{
	initCommands();
}

Server::~Server()
{
	this->clearAllResources();
}

void Server::initServer()
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		throw std::runtime_error("Error creating socket");

	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error setting socket to non-blocking");

	sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY; //default  0.0.0.0
	serverAddress.sin_port = htons(_port);

	if (bind(_serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
		throw std::runtime_error("Error binding socket");

	if (listen(_serverSocket, SOMAXCONN) == -1) // SOMAXCONN =4096
		throw std::runtime_error("Error listening");


	pollfd serverPoll;
	serverPoll.fd = _serverSocket;
	serverPoll.events = POLLIN; // POLLIN: Means "Check my mailbox, someone might have sent me a letter." (reading)
	serverPoll.revents = 0;
	_pollFds.push_back(serverPoll);

	std::cout << "Server successfully initialized on port " << _port << " :)" << std::endl;
}


void Server::runServer()
{
	std::cout << "Server is running and waiting for connections..." << std::endl;

	while(g_srvRunning)
	{
		for (size_t i = 0; i < _pollFds.size(); i++) {
			int currentFd = _pollFds[i].fd;
			if (currentFd != _serverSocket) {
				if (!_clients[currentFd]->getWriteBuffer().empty()) //buffer not empty , data jaya mn client
					_pollFds[i].events |= POLLOUT; //(writing)
				else
					_pollFds[i].events &= ~POLLOUT;
			}
		}

		if (poll(&_pollFds[0], _pollFds.size(), -1) == -1)
		{
			if(errno == EINTR)
			{
				if (g_srvRunning == 0)
				{
					std::cout << "\n[INFO] Shutdown signal caught. Escaping loop..." << std::endl;
					break;
				}
				continue;
			}
			throw std::runtime_error(std::string("poll: ") + strerror(errno));
		}

		for(size_t i = 0 ; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				handleClientDisconnect(i);
				continue;
			}
			if(_pollFds[i].revents & POLLIN) //1 & 1 = 1 > true
			{
				if (_pollFds[i].fd == _serverSocket)
					acceptNewClient();
				else
					receiveOldClientData(i);
			}
			if (_pollFds[i].revents & POLLOUT) //1 & 1 = 1 > true then send;
			{
				sendClientData(i);
			}
		}
	}
}

void Server::clearAllResources()
{
	std::cout << "[TEARDOWN] Freeing up memory and closing sockets..." << std::endl;

	// 1. Delete all Client heap allocations
	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete it->second;
	}
	_clients.clear();

	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd != -1)
		{
			close(_pollFds[i].fd);
			_pollFds[i].fd = -1;
		}
	}
	_pollFds.clear();

	if (_serverSocket != -1)
	{
		close(_serverSocket);
		_serverSocket = -1;
	}
}