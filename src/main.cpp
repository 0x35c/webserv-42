#include <iostream>
#include <csignal>
#include <cstdlib>

#include "Server.hpp"

void signal_handler(int signum)
{
	(void)signum;
	throw std::exception();
}

int	main(void)
{
	signal(SIGINT, signal_handler);

	Server server;
	try
	{
		server.addAddress("0.0.0.0", 8080);
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
