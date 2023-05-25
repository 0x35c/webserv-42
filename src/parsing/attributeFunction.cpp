#include "parsing.hpp"
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

#include <iostream>
void debug(t_location & location)
{
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
}

//LOCATION BLOCK ATTRIBUTE
void Parsing::testLocationValue()
{
	std::string tmpRoot = _location.root;
	if (tmpRoot[tmpRoot.length() - 1] != '/')
		tmpRoot = tmpRoot + "/";
	if (!isValidPathDir(tmpRoot))
		throw(ParsingError("line " + intToString(_location.lines[ROOT]) + " has an incorrect value."));
	if (!isValidPath(tmpRoot + _location.redirectionPath))
		throw(ParsingError("line " + intToString(_location.lines[REDIRECTION]) + " has an incorrect value."));
	if (!isValidPath(tmpRoot + _location.index))
		throw(ParsingError("line " + intToString(_location.lines[INDEX]) + " has an incorrect value."));
	if (!isValidPathDir(tmpRoot + _location.uploadedFilePath))
		throw(ParsingError("line " + intToString(_location.lines[UPLOAD]) + " has an incorrect value."));
}

void Parsing::CGIAttribute(const std::vector<std::string> & lineSplit)
{
	if (lineSplit.size() != 3)
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	if (access(lineSplit[1].c_str(), 0) != 0)
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	_location.executableCGI = lineSplit[1];
	_location.extensionCGI = lineSplit[2];
}

void Parsing::methodAttribute(const std::vector<std::string> & lineSplit)
{
	(void)lineSplit;
}

void Parsing::returnAttribute(const std::vector<std::string> & lineSplit)
{
	_location.redirectionCode = strtol(lineSplit[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError(strerror(errno)));
	else if (!isDigit(lineSplit[1]))
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	else if (lineSplit.size() < 3)
		throw(ParsingError("line " + intToString(_nbLine) + " has only one value and takes two."));
	_location.redirectionPath = lineSplit[2];
	_location.lines[REDIRECTION] = _nbLine;
}

void Parsing::rootAttribute(const std::vector<std::string> & lineSplit)
{
	_location.root = lineSplit[1];
	_location.lines[ROOT] = _nbLine;
}

void Parsing::directoryListingAttribute(const std::vector<std::string> & lineSplit)
{
	if (lineSplit[1] == "ON")
		_location.directoryListing = true;
	else if (lineSplit[1] == "OFF")
		_location.directoryListing = false;
	else
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
}

void Parsing::indexAttribute(const std::vector<std::string> & lineSplit)
{
	_location.index = lineSplit[1];
	_location.lines[INDEX] = _nbLine;
}
void Parsing::acceptUploadedFileAttribute(const std::vector<std::string> & lineSplit)
{
	if (lineSplit[1] == "ON")
		_location.acceptUploadedFile = true;
	else if (lineSplit[1] == "OFF")
		_location.acceptUploadedFile = false;
	else
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
}

void Parsing::saveUploadedFileAttribute(const std::vector<std::string> & lineSplit)
{
	_location.uploadedFilePath = lineSplit[1];
	_location.lines[UPLOAD] = _nbLine;
}

//SERVER BLOCK ATTRIBUTE

void Parsing::listenAttribute(const std::vector<std::string> & lineSplit)
{
	if (lineSplit.size() < 3)
		throw(ParsingError("line " + intToString(_nbLine) + " has only one value and takes two."));
	std::vector<std::string> portSplit = splitString(lineSplit[2], '.');
	if (portSplit.size() != 4 || countChar(lineSplit[2], '.') != 3)
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	for (size_t j = 0; j < 4; j++)
	{
		if (!isDigit(portSplit[j]))
			throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	}
	_server.host = lineSplit[2];
	if (!isDigit(lineSplit[1]))
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	_server.port = strtol(lineSplit[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
}

void Parsing::serverNameAttribute(const std::vector<std::string> & lineSplit)
{
	(void)_nbLine;
	_server.server_name = lineSplit[1];
}

void Parsing::errpageAttribute(const std::vector<std::string> & lineSplit)
{
	if (!isValidPath(lineSplit[1]))
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	_server.errpage = lineSplit[1];
}

void Parsing::maxFilesizeUploadAttribute(const std::vector<std::string> & lineSplit)
{
	_server.maxFileSizeUpload = strtol(lineSplit[1].c_str(), NULL, 10);
	if (errno != 0)
		throw(ParsingError(strerror(errno)));
	else if (!isDigit(lineSplit[1]))
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
}

void Parsing::locationAttribute(const std::vector<std::string> & lineSplit)
{
	initializeLocation();
	if (!isValidPath(lineSplit[1]))
		throw(ParsingError("line " + intToString(_nbLine) + " has an incorrect value."));
	_location.locationPath = lineSplit[1];
	_server.locations.push_back(_location);
}
