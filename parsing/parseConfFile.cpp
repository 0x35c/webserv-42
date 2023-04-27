#include "parsing.hpp"
#include <fstream>

const std::vector<t_server> readConfFile(std::ifstream & conf_file)
{
	std::vector<t_server> servers;
	//find instantiation of a server
	//push back one in the vector and initialize it
	//search for parameter in the server {}
	//if a line isn't conform, stop

	int 						nbLine = 0;
	bool 						inServerBlock = false;
	bool 						inLocationBlock = false;
	bool 						inMethodBlock = false;
	std::string 				line;
	t_server					server;
	t_location					location;
	while (std::getline(conf_file, line))
	{
		nbLine++;
		trimString(line);
		if (line[0] == '#' && line[1] == '#')
			continue ;
		if (!inServerBlock && !inLocationBlock && !inMethodBlock)
		{
			if (line.length() > 0 && line != "server {")
				throw(ParsingError("line " + intToString(nbLine) + " is invalid."));
			inServerBlock = true;
			initializeServer(server);
			continue;
		}
		if (inServerBlock && !inLocationBlock && !inMethodBlock)
		{
			if (line.length() == 0)
				continue;
			else if (line.find("location") != std::string::npos) //problem if a value contain "location"
				inLocationBlock = true;
			else if (line == "}")
				inServerBlock = false;
			parseLineServerBlock(line, nbLine, server);
			continue ;
		}
		if (inServerBlock && inLocationBlock && !inMethodBlock)
		{
			if (line.length() == 0)
				continue;
			else if (line.find("methods") != std::string::npos) //problem if a value contain "methods"
				inMethodBlock = true;
			else if (line == "}")
				inLocationBlock = false;
			parseLineLocationBlock(line, nbLine, location);
			continue ;
		}
		if (inServerBlock && inLocationBlock && inMethodBlock)
		{
			if (line.length() == 0)
				continue;
			else if (line == "}")
				inMethodBlock = false;
			parseLineMethodBlock(line, nbLine, location);
			continue ;
		}
	}
	conf_file.close();
	return (servers);
}

const std::vector<t_server> parseConfFile(const std::string path)
{
	if (path.find(".conf") + 5 != path.length())
		throw (ParsingError("configuration file must have \".conf\" extension."));
	std::ifstream confFile(path.c_str(), std::ios_base::in);
	if (!confFile.is_open())
		throw (ParsingError("can't open configuration file."));
	return (readConfFile(confFile));
}
