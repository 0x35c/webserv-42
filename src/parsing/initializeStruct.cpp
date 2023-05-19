#include "parsing.hpp"

void	Parsing::initializeLocation(t_location & location)
{
	location.locationPath = ""; // "/" ?
	for (int i = 0; i < 3; i++)
		location.methodsAllowed[i] = true;
	location.redirectionCode = 0; // ?
	location.redirectionPath = ""; // ?ZZ
	location.root = ""; // locationPath ?
	location.directoryListing = true;
	location.index = ""; // ? {locationPath}/index.html => error page if doesn't exist ?
	location.acceptUploadedFile = true;
	location.uploadedFilePath = ""; // {locationPath}/uploads/ => need to create directory || www/uploads/ ?
}

void	Parsing::initializeServer(t_server & server)
{
	server.host = "0.0.0.0";
	server.port = 8080;
	server.server_name = "server";
	server.isDefaultServer = false; // ?
	server.errpage = ""; // www/error.html ?
	server.maxFileSizeUpload = 0; // 100 (Mo)
}
