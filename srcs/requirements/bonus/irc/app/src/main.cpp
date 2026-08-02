#include "server.hpp"

volatile sig_atomic_t g_srvRunning = 1;

void sig_handel(int)
{
	g_srvRunning = 0;
}
int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	int port = std::atoi(argv[1]);
	if (port < 1024 || port > 65535)
	{
		std::cerr << "Error: Port must be a number between 1024 and 65535." << std::endl;
		return 1;
	}
	std::string password = argv[2];

	std::signal(SIGINT, sig_handel);	// Catches Ctrl+C
	std::signal(SIGTERM, sig_handel);	// Catches termination commands (e.g., kill)
	std::signal(SIGPIPE, SIG_IGN);		// Ignores broken pipe crashes

	try
	{
		Server myIrc(port, password);
		myIrc.initServer();
		myIrc.runServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << "An error occurred: " << e.what() << std::endl;
	}
	return 0;
}
