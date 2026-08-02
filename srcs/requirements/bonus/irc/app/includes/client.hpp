#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

class Client
{
	private:
		int		    _fd;
		std::string _readBuffer;
		std::string _writeBuffer;
		std::string _hostname;

        // --- ADDED FOR IRC PROTOCOL STATE ---
        bool        _isAuthenticated; // True if they sent the correct PASS
        bool        _isRegistered;    // True if PASS, NICK, and USER are all done
        std::string _nickname;
        std::string _username;
        std::string _realname;

	public:
		Client(int fd) ;
		~Client();

		int getFd() const;

		std::string& getReadBuffer();
		void appendToReadBuffer(std::string data) ;

		std::string& getWriteBuffer();
		void appendToWriteBuffer(std::string data);
		void clearWriteBuffer() ;


        // --- ADDED GETTERS & SETTERS FOR COMMAND LOGIC ---

        bool isRegistered() const ;
        void setRegistered(bool status);
        bool isAuthenticated() const ;
        void setAuthenticated(bool status);
        const std::string& getNickname() const ;
        void setNickname(const std::string& nick) ;
        const std::string& getUsername() const ;
        void setUsername(const std::string& user);
        const std::string& getRealname() const ;
        void setRealname(const std::string& realname) ;

		void setHostname(const std::string& ip);
		std::string getHostname() const;

};

#endif
