#include "parsing.hpp"

Parsing::Parsing()
{
	_inServerBlock = false;
	_inLocationBlock = false;
	_inMethodBlock = false;
	resetBlockArg(SERVER_BLOCK);
	resetBlockArg(LOCATION_BLOCK);
	resetBlockArg(METHOD_BLOCK);
}

Parsing::~Parsing()
{
}
