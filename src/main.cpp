#include <iostream>
#include <csignal>
#include <cstdlib>

#include "Server.hpp"
#include "parsing/parsing.hpp"

void signal_handler(int signum)
{
	(void)signum;
	throw std::runtime_error("SIGINT received");
}

int	main(int argc, char **argv)
{
	signal(SIGINT, signal_handler);
	
	if (argc != 2)
	{
		std::cout << "an argument is needed\n";
		return EXIT_FAILURE;
	}
	std::vector<t_server> serverConfigFile;
	try
	{
		serverConfigFile = Parsing::parseConfFile(argv[1]);
	}
	catch(const std::string exception)
	{
		std::cerr << exception;
		return EXIT_FAILURE;
	}
	Server server;
	try
	{
		for (size_t i = 0; i < serverConfigFile.size(); i++)
			server.addAddress(serverConfigFile[i].host, serverConfigFile[i].port);
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
