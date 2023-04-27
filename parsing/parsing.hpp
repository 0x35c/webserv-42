#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <vector>

typedef struct location {
	std::string	locationPath;
	bool 		methodsAllowed[3];
	int			redirectionCode;
	std::string	redirectionPath;
	std::string	root;
	bool		directoryListing;
	std::string	index;
	bool		acceptUploadedFile;
	std::string	uploadedFilePath;
	//add CGI content

} t_location;

typedef struct server {
	std::string 			address; //address = host:port
	std::string 			server_name;
	bool					isDefaultServer;
	std::string 			errpage;
	int						maxFileSizeUpload;
	std::vector<t_location>	locations;
} t_server;

//parsing.cpp
const std::vector<server> parseConfFile(const std::string path);

//parseLine.hpp

void	parseLineServerBlock(std::string & line, int nbLine, t_server & server);
void	parseLineLocationBlock(std::string & line, int nbLine, t_location & location);
void	parseLineMethodBlock(std::string & line, int nbLine, t_location & location);

//initializeStruct.cpp

void	initializeServer(t_server & server);
void	initializeLocation(t_location & location);

//utils.cpp

bool 							isDigit(const std::string string);
bool 							isValidPath(const std::string string);
void							trimString(std::string & string);
const std::string 				ParsingError(std::string error) throw();
const std::string				intToString(const int number);
const std::vector<std::string>	splitString(const std::string string, const char delimiter);

#endif
