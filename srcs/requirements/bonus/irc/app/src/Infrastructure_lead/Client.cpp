#include "client.hpp"

Client::Client(int fd) : _fd(fd), _readBuffer(""), _writeBuffer(""), 
                 _isAuthenticated(false), _isRegistered(false), 
                       _nickname(""), _username(""), _realname("") {}

Client::~Client() {}

int Client::getFd() const
{
	return _fd;
}

std::string& Client::getReadBuffer()
{
	return _readBuffer;
}

std::string& Client::getWriteBuffer()
{
	return _writeBuffer;
}

void Client::appendToReadBuffer(std::string data)
{
	_readBuffer += data;
}

void Client::appendToWriteBuffer(std::string data)
{
	_writeBuffer += data;
}

void Client::clearWriteBuffer()
{
	_writeBuffer.clear();
}

bool Client::isRegistered() const 
	{ 
			return _isRegistered; 
		}
void Client::setRegistered(bool status)
	{ 
			_isRegistered = status; 
		}

bool Client::isAuthenticated() const 
	{ 
			return _isAuthenticated; 
		}
void Client::setAuthenticated(bool status)
	{
			 _isAuthenticated = status; 
		}

const std::string& Client::getNickname() const 
	{ 
			return _nickname; 
		}
void Client::setNickname(const std::string& nick) 
	{ 
			_nickname = nick; 
		}

const std::string& Client::getUsername() const 
	{ 
			return _username; 
		}
void Client::setUsername(const std::string& user)
	{ 
			_username = user; 
		}

const std::string& Client::getRealname() const 
	{ 
			return _realname; 
		}
void Client::setRealname(const std::string& realname) 
	{ 
			_realname = realname; 
		}