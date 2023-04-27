#include "parsing.hpp"
#include <fstream>
#include <cstring>

const std::vector<t_server> readConfFile(std::ifstream & conf_file)
{
	std::vector<t_server> servers;

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
			else if (strncmp(line.c_str(), "location", 8) == 0)
				inLocationBlock = true;
			else if (line == "}")
			{
				servers.push_back(server);
				inServerBlock = false;
			}
			if (line != "}")
				parseLineServerBlock(line, nbLine, server);
			continue ;
		}
		if (inServerBlock && inLocationBlock && !inMethodBlock)
		{
			if (line.length() == 0)
				continue;
			else if (strncmp(line.c_str(), "methods", 7) == 0)
				inMethodBlock = true;
			else if (line == "}")
			{
				server.locations.push_back(location);
				inLocationBlock = false;
			}
			if (line != "}")
				parseLineLocationBlock(line, nbLine, location);
			continue ;
		}
		if (inServerBlock && inLocationBlock && inMethodBlock)
		{
			if (line.length() == 0)
				continue;
			else if (line == "}")
				inMethodBlock = false;
			if (line != "}")
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
