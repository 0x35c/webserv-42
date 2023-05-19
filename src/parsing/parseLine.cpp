#include "parsing.hpp"
#include <cstring>
#include <cstdlib>
#include <cerrno>

void	parseLineMethodBlock(std::string & line, int nbLine, t_location & location)
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

void	parseLineLocationBlock(std::string & line, int nbLine, t_location & location)
{
	std::string arguments[8] = { "methods", "return", "root", "directory_listing", "index", "", "accept_uploaded_file", "save_uploaded_file" };
	std::vector<std::string> lineSplitted = splitString(line, ' ');
	if (lineSplitted.size() < 2)
		throw(ParsingError("line " + intToString(nbLine) + " has no value and take at least one."));
	for (int i = 0; i < 8; i++)
	{
		if (arguments[i] == lineSplitted[0])
		{
			switch (i)
			{
				case 0:
					//need to see what to do here for methods;
					return;
				case 1:
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
					return;
				case 2:
					if (!isValidPath(lineSplitted[1]))
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					location.root = lineSplitted[1];
					return;
				case 3:
					if (lineSplitted[1] == "ON")
						location.directoryListing = true;
					else if (lineSplitted[1] == "OFF")
						location.directoryListing = false;
					else
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					return;
				case 4:
					if (!isValidPath(lineSplitted[1]))
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					location.index = lineSplitted[1];
					return;
				case 5:
					//CGI
					return;
				case 6:
					if (lineSplitted[1] == "ON")
						location.acceptUploadedFile = true;
					else if (lineSplitted[1] == "OFF")
						location.acceptUploadedFile = false;
					else
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					return;
				case 7:
					if (!isValidPath(lineSplitted[1]))
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					location.uploadedFilePath = lineSplitted[1];
					return;
			}
		}
	}
	throw(ParsingError("line " + intToString(nbLine) + " has an incorrect variable declaration."));
}

void	parseLineServerBlock(std::string & line, int nbLine, t_server & server)
{
	std::string arguments[5] = { "listen", "server_name", "errpage", "max_filesize_upload", "location" };
	std::vector<std::string> lineSplitted = splitString(line, ' ');
	if (lineSplitted.size() < 2)
		throw(ParsingError("line " + intToString(nbLine) + " has no value and takes at least one."));
	for (int i = 0; i < 5; i++)
	{
		if (arguments[i] == lineSplitted[0])
		{
			switch (i)
			{
				case 0:
					if (lineSplitted.size() < 3)
						throw(ParsingError("line " + intToString(nbLine) + " has only one value and takes two."));
					server.address = lineSplitted[1] + ":" + lineSplitted[2];
					return;
				case 1:
					server.server_name = lineSplitted[1];
					return;
				case 2:
					if (!isValidPath(lineSplitted[1]))
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					server.errpage = lineSplitted[1];
					return;
				case 3:
					server.maxFileSizeUpload = strtol(lineSplitted[1].c_str(), NULL, 10);
					if (errno != 0)
						throw(ParsingError(strerror(errno)));
					else if (!isDigit(lineSplitted[1]))
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					return;
				case 4:
					t_location location;
					initializeLocation(location);
					if (!isValidPath(lineSplitted[1]))
						throw(ParsingError("line " + intToString(nbLine) + " has an incorrect value."));
					location.locationPath = lineSplitted[1];
					return;
			}
		}
	}
	throw(ParsingError("line " + intToString(nbLine) + " has an incorrect variable declaration."));
}
