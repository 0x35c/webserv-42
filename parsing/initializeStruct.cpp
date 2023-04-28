#include "parsing.hpp"

void	Parsing::initializeLocation(t_location & location)
{
	location.locationPath = "";
	for (int i = 0; i < 3; i++)
		location.methodsAllowed[i] = false;
	location.redirectionCode = 0;
	location.redirectionPath = "";
	location.root = "";
	location.directoryListing = false;
	location.index = "";
	location.acceptUploadedFile = false;
	location.uploadedFilePath = "";
}

void	Parsing::initializeServer(t_server & server)
{
	server.host = "";
	server.port = 0;
	server.server_name = "";
	server.isDefaultServer = false;
	server.errpage = "";
	server.maxFileSizeUpload = 0;
}
