#include "parsing.hpp"

void	Parsing::parseLineMethodBlock(const std::string & line, const int & nbLine, t_location & location)
{
	std::string arguments[3] = {"GET", "POST", "DELETE"};
	std::vector<std::string> lineSplitted = splitString(line, ' ');
	if (lineSplitted.size() < 2)
		throw(ParsingError("line " + intToString(nbLine) + " has no value and take one."));
	for (int i = 0; i < 3; i++)
	{
		if (arguments[i] == lineSplitted[0])
		{
			if (lineSplitted[1] == "ON")
				location.methodsAllowed[i] = true;
			else if (lineSplitted[1] == "OFF")
				location.methodsAllowed[i] = false;
			else
				throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
			return;
		}
	}
	throw(ParsingError("line " + intToString(nbLine) + " has an incorrect variable declaration."));
}

void	Parsing::parseLineLocationBlock(const std::string & line, const int & nbLine, t_location & location)
{
	std::string arguments[8] = { "methods", "return", "root", "directory_listing", "index", "", "accept_uploaded_file", "save_uploaded_file" };
	bool argumentsUsed[8] = {false};
	void (*argumentsFunctions[8])(t_location &, const int &, const std::vector<std::string> &) = {&methodAttribute, &returnAttribute, &rootAttribute, &directoryListingAttribute, &indexAttribute, NULL, &acceptUploadedFileAttribute, &saveUploadedFileAttribute};
	std::vector<std::string> lineSplitted = splitString(line, ' ');
	
	if (lineSplitted.size() < 2)
		throw(ParsingError("line " + intToString(nbLine) + " has no value and take at least one."));
	for (int i = 0; i < 8; i++)
	{
		if (arguments[i] == lineSplitted[0])
		{
			if (argumentsUsed[i])
				throw(ParsingError("line " + intToString(nbLine) + " contains a redeclaration of an attribute."));
			argumentsUsed[i] = true;
			argumentsFunctions[i](location, nbLine, lineSplitted);
			return ;
		}
	}
	throw(ParsingError("line " + intToString(nbLine) + " has an incorrect variable declaration."));
}

#include <iostream>

void	Parsing::parseLineServerBlock(const std::string & line, const int & nbLine, t_server & server)
{
	std::string arguments[5] = { "listen", "server_name", "errpage", "max_filesize_upload", "location" };
	bool argumentsUsed[5] = {false};
	void (*argumentsFunctions[5])(t_server &, const int &, const std::vector<std::string> &) = {&listenAttribute, &serverNameAttribute, &errpageAttribute, &maxFilesizeUploadAttribute, &locationAttribute};
	std::vector<std::string> lineSplitted = splitString(line, ' ');
	if (lineSplitted.size() < 2)
		throw(ParsingError("line " + intToString(nbLine) + " has no value and takes at least one."));
	for (int i = 0; i < 5; i++)
	{
		if (arguments[i] == lineSplitted[0])
		{
			if (argumentsUsed[i])
				throw(ParsingError("line " + intToString(nbLine) + " contains a redeclaration of an attribute."));
			argumentsUsed[i] = true;
			argumentsFunctions[i](server, nbLine, lineSplitted);
			return ;
		}
	}
	throw(ParsingError("line " + intToString(nbLine) + " has an incorrect variable declaration."));
}
