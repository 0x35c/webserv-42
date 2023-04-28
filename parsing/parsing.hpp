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
	std::string 			host;
	int						port;
	std::string 			server_name;
	bool					isDefaultServer;
	std::string 			errpage;
	int						maxFileSizeUpload;
	std::vector<t_location>	locations;
} t_server;

//parsing.cpp
const std::vector<server> parseConfFile(const std::string path);

//parseLine.cpp

void	parseLineServerBlock(const std::string & line, const int & nbLine, t_server & server);
void	parseLineLocationBlock(const std::string & line, const int & nbLine, t_location & location);
void	parseLineMethodBlock(const std::string & line, const int & nbLine, t_location & location);

//attributeFunction.cpp

void methodAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted);
void returnAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted);
void rootAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted);
void directoryListingAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted);
void indexAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted);
void acceptUploadedFileAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted);
void saveUploadedFileAttribute(t_location & location, const int & nbLine, const std::vector<std::string> & lineSplitted);

void listenAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted);
void serverNameAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted);
void errpageAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted);
void maxFilesizeUploadAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted);
void locationAttribute(t_server & server, const int & nbLine, const std::vector<std::string> & lineSplitted);

//initializeStruct.cpp

void	initializeServer(t_server & server);
void	initializeLocation(t_location & location);

//utils.cpp

bool 							isDigit(const std::string string);
bool 							isValidPath(const std::string string);
void							trimString(std::string & string);
size_t 							countChar(const std::string string, const char delimiter);
const std::string 				ParsingError(std::string error) throw();
const std::string				intToString(const int number);
const std::vector<std::string>	splitString(const std::string string, const char delimiter);

#endif
