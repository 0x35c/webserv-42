#include "parsing.hpp"

void	Parsing::initializeLocation()
{ 
	for (int i = 0; i < 3; i++)
		_location.methodsAllowed[i] = true;
	_location.redirectionCode = 0; // ?
	_location.redirectionPath = ""; // ?
	_location.root = ""; // _locationPath ?
	_location.directoryListing = true;
	_location.executableCGI = "/usr/bin/python3";
	_location.extensionCGI = ".py";
	_location.index = ""; // ? {_locationPath}/index.html => error page if doesn't exist ?
	_location.acceptUploadedFile = true;
	_location.uploadedFilePath = ""; // {_locationPath}/uploads/ => need to create directory || www/uploads/ ?
}

void	Parsing::initializeServer()
{
	_server.host = "0.0.0.0";
	_server.port = 8080;
	_server.server_name = "_server";
	_server.isDefaultServer = false; // ?
	_server.errpage = ""; // www/error.html ?
	_server.maxFileSizeUpload = 0; // 100 (Mo)
}
