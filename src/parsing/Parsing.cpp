#include "parsing.hpp"

Parsing::Parsing()
{
	_nbLine = 0;
	_inServerBlock = false;
	_inLocationBlock = false;
	_inMethodBlock = false;
	resetBlockArg(SERVER_BLOCK);
	resetBlockArg(LOCATION_BLOCK);
	resetBlockArg(METHOD_BLOCK);
}

Parsing::~Parsing()
{
	for (std::vector<t_server*>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		for (std::vector<t_location*>::iterator itVector = (*it)->locations.begin(); itVector != (*it)->locations.end(); ++itVector)
			delete *itVector;
		delete *it;
	}
	delete _server;
	delete _location;
}
