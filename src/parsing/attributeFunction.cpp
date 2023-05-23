#include "parsing.hpp"
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

//#include <iostream>

//LOCATION BLOCK ATTRIBUTE
void Parsing::testLocationValue(const t_location & location)
{
	std::string tmpRoot = location.root;
	if (tmpRoot[tmpRoot.length() - 1] != '/')
		tmpRoot = tmpRoot + "/";
	if (!isValidPathDir(tmpRoot))
		throw(ParsingError("line " + intToString(location.lines[ROOT]) + " has an incorrect value."));
	if (!isValidPath(tmpRoot + location.redirectionPath))
		throw(ParsingError("line " + intToString(location.lines[REDIRECTION]) + " has an incorrect value."));
	if (!isValidPath(tmpRoot + location.index))
		throw(ParsingError("line " + intToString(location.lines[INDEX]) + " has an incorrect value."));
	if (!isValidPathDir(tmpRoot + location.uploadedFilePath))
		throw(ParsingError("line " + intToString(location.lines[UPLOAD]) + " has an incorrect value."));
	
	/*
	std::cout << "LOCATION PATH:" + location.locationPath + "\n";
	std::cout << "ROOT:" + location.root + "\n";
	std::cout << "INDEX:" + location.index + "\n";
	std::cout << "DIRECTORY LISTING:" << location.directoryListing << "\n";
	std::cout << "REDIRECTION PATH:" << location.redirectionPath << "\n";
	std::cout << "REDIRECTION CODE:" << location.redirectionCode << "\n";
	std::cout << "ACCEPT UPLOADED FILE:" << location.acceptUploadedFile << "\n";
	std::cout << "UPLOADED FILE PATH:" + location.uploadedFilePath + "\n";
	std::cout << "GET:" << location.methodsAllowed[0] << "\n";
	std::cout << "POST:" << location.methodsAllowed[1] << "\n";
	std::cout << "DELETE:" << location.methodsAllowed[2] << "\n";
	*/
}

void Parsing::CGIAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (lineSplitted.size() != 3)
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	if (access(lineSplitted[1].c_str(), 0) != 0)
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	location.executableCGI = lineSplitted[1];
	location.extensionCGI = lineSplitted[2];
}

void Parsing::methodAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	(void)location, (void)nbLine, (void)lineSplitted;
}

void Parsing::returnAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	location.redirectionCode = strtol(lineSplitted[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError(strerror(errno)));
	else if (!isDigit(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	else if (lineSplitted.size() < 3)
		throw(ParsingError("line " + intToString(nbLine) + " has only one value and takes two."));
	location.redirectionPath = lineSplitted[2];
	location.lines[REDIRECTION] = nbLine;
}

void Parsing::rootAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	location.root = lineSplitted[1];
	location.lines[ROOT] = nbLine;
}

void Parsing::directoryListingAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (lineSplitted[1] == "ON")
		location.directoryListing = true;
	else if (lineSplitted[1] == "OFF")
		location.directoryListing = false;
	else
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void Parsing::indexAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	location.index = lineSplitted[1];
	location.lines[INDEX] = nbLine;
}
void Parsing::acceptUploadedFileAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (lineSplitted[1] == "ON")
		location.acceptUploadedFile = true;
	else if (lineSplitted[1] == "OFF")
		location.acceptUploadedFile = false;
	else
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void Parsing::saveUploadedFileAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	location.uploadedFilePath = lineSplitted[1];
	location.lines[UPLOAD] = nbLine;
}

//SERVER BLOCK ATTRIBUTE

void Parsing::listenAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (lineSplitted.size() < 3)
		throw(ParsingError("line " + intToString(nbLine) + " has only one value and takes two."));
	std::vector<std::string> portSplitted = splitString(lineSplitted[2], '.');
	if (portSplitted.size() != 4 || countChar(lineSplitted[2], '.') != 3)
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	for (size_t j = 0; j < 4; j++)
	{
		if (!isDigit(portSplitted[j]))
			throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	}
	server.host = lineSplitted[2];
	if (!isDigit(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	server.port = strtol(lineSplitted[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void Parsing::serverNameAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	(void)nbLine;
	server.server_name = lineSplitted[1];
}

void Parsing::errpageAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	if (!isValidPath(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	server.errpage = lineSplitted[1];
}

void Parsing::maxFilesizeUploadAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	server.maxFileSizeUpload = strtol(lineSplitted[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError(strerror(errno)));
	else if (!isDigit(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
}

void Parsing::locationAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted)
{
	t_location location;
	initializeLocation(location);
	if (!isValidPath(lineSplitted[1]))
		throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
	location.locationPath = lineSplitted[1];
	server.locations.push_back(location);
}
