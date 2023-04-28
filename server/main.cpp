#include "Server.hpp"
#include "../parsing/parsing.hpp"
#include <iostream>
//start program and connect to "localhost:8080"

void checkValueParsing(std::vector<t_server> servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "server : " << i << "\n";
		std::cout << "listen : " << servers[i].host << " " << servers[i].port << "\n";
		std::cout << "server_name : " << servers[i].server_name << "\n";
		std::cout << "isDefaultServer : " << servers[i].isDefaultServer << "\n";
		std::cout << "errpage : " << servers[i].errpage << "\n";
		std::cout << "max_filesize_upload : " << servers[i].maxFileSizeUpload << "\n";
		for (size_t j = 0; j < servers[i].locations.size(); j++)
		{
			std::cout << "location :" << j << "\n";
			std::cout << "method GET : " << servers[i].locations[j].methodsAllowed[0] << "\n";
			std::cout << "method POST : " << servers[i].locations[j].methodsAllowed[1] << "\n";
			std::cout << "method DELETE : " << servers[i].locations[j].methodsAllowed[2] << "\n";
			std::cout << "return : " << servers[i].locations[j].redirectionCode << " " << servers[i].locations[j].redirectionPath << "\n";
			std::cout << "root : " << servers[i].locations[j].root << "\n";
			std::cout << "directory_listing : " << servers[i].locations[j].directoryListing << "\n";
			std::cout << "index : " << servers[i].locations[j].index << "\n";
			std::cout << "accept_uploaded_file : " << servers[i].locations[j].acceptUploadedFile << "\n";
			std::cout << "save_uploaded_file : " << servers[i].locations[j].uploadedFilePath << "\n";
		}
	}
}

int	main(int ac, char **av)
{
	try
	{
		if (ac != 2)
			throw (ParsingError("only one parameter accepted."));
		(void)av;
		/* std::vector<t_server> servers = parseConfFile(av[1]); */
		Server server("0.0.0.0", 8080);
	}
	catch (std::string & exception)
	{
		std::cerr << exception;
	}

	return (EXIT_SUCCESS);
}
