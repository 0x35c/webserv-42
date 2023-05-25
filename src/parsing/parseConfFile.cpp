#include "parsing.hpp"
#include <fstream>
#include <cstring>

void	Parsing::checkDifferentServer()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		for (size_t j = i + 1; j < _servers.size(); j++)
		{
			if (_servers[i].port == _servers[j].port && _servers[i].host == _servers[j].host)
				throw (ParsingError("multiple server with same address."));
		}
	}
}

void	Parsing::noBlock()
{
	if (_line.length() > 0 && _line != "server {")
		throw(ParsingError("line " + intToString(_nbLine) + " is invalid."));
	_inServerBlock = true;
	initializeServer();
}

void Parsing::serverBlock()
{
			if (strncmp(_line.c_str(), "location", 8) == 0)
			{
				std::vector<std::string> lineSplitted = splitString(_line, ' ');
				if (lineSplitted.size() != 3)
					throw(ParsingError("line " + intToString(_nbLine) + " is invalid."));
				_location.locationPath = lineSplitted[1];
				initializeLocation();
				_inLocationBlock = true;
			}
			else if (_line == "}")
			{
				_servers.push_back(_server);
				_inServerBlock = false;
				resetBlockArg(SERVER_BLOCK);
			}
			else
				parseLineServerBlock();
}

void Parsing::locationBlock()
{
			if (strncmp(_line.c_str(), "methods", 7) == 0)
				_inMethodBlock = true;
			else if (_line == "}")
			{
				testLocationValue();
				_server.locations.push_back(_location);
				_inLocationBlock = false;
				resetBlockArg(LOCATION_BLOCK);
			}
			else
				parseLineLocationBlock();
}

void Parsing::methodBlock()
{
			if (_line == "}")
			{
				_inMethodBlock = false;
				resetBlockArg(METHOD_BLOCK);
			}
			else
				parseLineMethodBlock();
}

const std::vector<t_server> Parsing::readConfFile(std::ifstream & confFile)
{
	while (std::getline(confFile, _line))
	{
		_nbLine++;
		trimString(_line, WHITESPACE);
		_line = _line.substr(0, _line.find("##"));
		if (_line.length() == 0)
			continue;
		else if (!_inServerBlock && !_inLocationBlock && !_inMethodBlock)
			noBlock();
		else if (_inServerBlock && !_inLocationBlock && !_inMethodBlock)
			serverBlock();
		else if (_inServerBlock && _inLocationBlock && !_inMethodBlock)
			locationBlock();
		else if (_inServerBlock && _inLocationBlock && _inMethodBlock)
			methodBlock();
	}
	confFile.close();
	checkDifferentServer();
	return (_servers);
}

const std::vector<t_server> Parsing::parseConfFile(const std::string path)
{
	if (path.find(".conf") + 5 != path.length())
		throw (ParsingError("configuration file must have \".conf\" extension."));
	std::ifstream confFile(path.c_str(), std::ios_base::in);
	if (!confFile.is_open())
		throw (ParsingError("can't open configuration file."));
	return (readConfFile(confFile));
}
