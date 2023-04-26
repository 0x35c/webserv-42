#include "Server.hpp"
#include "../parsing/parsing.hpp"
//start program and connect to "localhost:8080"
int	main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
			throw (ParsingError("only one parameter accepted."));
		std::vector<server> servers;
		parse_conf_file(argv[1]);
		Server server("0.0.0.0", 8080);
	}
	catch (std::string & exception)
	{
		std::cerr << exception;
	}

	return (EXIT_SUCCESS);
}
