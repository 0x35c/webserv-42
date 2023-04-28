#include "parsing.hpp"
#include <cstring>
#include <cerrno>
#include <cstdlib>

//LOCATION BLOCK ATTRIBUTE
void methodAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	(void)location, (void)nbLine, (void)lineSplitted;
}

void returnAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	location.redirectionCode = strtol(lineSplitted[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError(strerror(errno)));
	else if (!isDigit(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	else if (lineSplitted.size() < 3)
		throw(ParsingError("line " + intToString(nbLine) + " has only one value and takes two."));
	else if (!isValidPath(lineSplitted[2]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	location.redirectionPath = lineSplitted[2];
}

void rootAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (!isValidPath(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	location.root = lineSplitted[1];
}

void directoryListingAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (lineSplitted[1] == "ON")
		location.directoryListing = true;
	else if (lineSplitted[1] == "OFF")
		location.directoryListing = false;
	else
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void indexAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (!isValidPath(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	location.index = lineSplitted[1];
}

void acceptUploadedFileAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (lineSplitted[1] == "ON")
		location.acceptUploadedFile = true;
	else if (lineSplitted[1] == "OFF")
		location.acceptUploadedFile = false;
	else
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void saveUploadedFileAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (!isValidPath(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	location.uploadedFilePath = lineSplitted[1];
}

//SERVER BLOCK ATTRIBUTE

void listenAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (lineSplitted.size() < 3)
		throw(ParsingError("line " + intToString(nbLine) + " has only one value and takes two."));
	std::vector<std::string> portSplitted = splitString(lineSplitted[1], '.');
	if (portSplitted.size() != 4 || countChar(lineSplitted[1], '.') != 3)
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	for (size_t j = 0; j < 4; j++)
	{
		if (!isDigit(portSplitted[j]))
			throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	}
	server.host = lineSplitted[1];
	if (!isDigit(lineSplitted[2]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	server.port = strtol(lineSplitted[2].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void serverNameAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	(void)nbLine;
	server.server_name = lineSplitted[1];
}

void errpageAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (!isValidPath(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	server.errpage = lineSplitted[1];
}

void maxFilesizeUploadAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	server.maxFileSizeUpload = strtol(lineSplitted[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError(strerror(errno)));
	else if (!isDigit(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void locationAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	t_location location;
	initializeLocation(location);
	if (!isValidPath(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	location.locationPath = lineSplitted[1];
	server.locations.push_back(location);
}
