#pragma once

#define WHITESPACE " \n\r\t\f\v"

#define ROOT 0
#define REDIRECTION 1
#define UPLOAD 2
#define INDEX 3

#define SERVER_BLOCK 0
#define LOCATION_BLOCK 1
#define METHOD_BLOCK 2

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <map>

typedef struct s_location {
	std::string	locationPath;
	bool 		methodsAllowed[3];
	int			redirectionCode;
	std::string	redirectionPath;
	std::string	root;
	bool		directoryListing;
	std::string	index;
	bool		acceptUploadedFile;
	std::string	uploadedFilePath;
	int			lines[4];
	std::string executableCGI;
	std::string extensionCGI;

} t_location;


typedef struct s_server {
	std::string 			host;
	int						port;
	std::string 			server_name;
	bool					isDefaultServer;
	std::string 			errpage;
	int						maxFileSizeUpload;
	sockaddr_in				socketAddress;
	std::vector<t_location>	locations;
} t_server;

class Parsing;

typedef void (Parsing::*function)(const std::vector<std::string> &);

class Parsing
{
	public:
		//parseConfFile.cpp
		
		const std::vector<t_server> parseConfFile(const std::string path);
		
		//Parsing.cpp
		
		Parsing();
		~Parsing();
	
	private:
		//Parsing.cpp
		
		Parsing(const Parsing & other);
		Parsing & operator = (const Parsing & other);
		
		//parseConfFile.cpp
		 void noBlock();
		 void serverBlock();
		 void locationBlock();
		 void methodBlock();
		 const std::vector<t_server> readConfFile(std::ifstream & confFile);
		 void checkDifferentServer();

		//parseLine.cpp

		 std::map<std::string, function> initFunctionMap(int block);
		 void	resetBlockArg(int block);
		 void	parseLineServerBlock();
		 void	parseLineLocationBlock();
		 void	parseLineMethodBlock();

		//attributeFunction.cpp

		 void testLocationValue();
		 void CGIAttribute(const std::vector<std::string> & lineSplit);
		 void methodAttribute(const std::vector<std::string> & lineSplit);
		 void returnAttribute(const std::vector<std::string> & lineSplit);
		 void rootAttribute(const std::vector<std::string> & lineSplit);
		 void directoryListingAttribute(const std::vector<std::string> & lineSplit);
		 void indexAttribute(const std::vector<std::string> & lineSplit);
		 void acceptUploadedFileAttribute(const std::vector<std::string> & lineSplit);
		 void saveUploadedFileAttribute(const std::vector<std::string> & lineSplit);

		 void listenAttribute(const std::vector<std::string> & lineSplit);
		 void serverNameAttribute(const std::vector<std::string> & lineSplit);
		 void errpageAttribute(const std::vector<std::string> & lineSplit);
		 void maxFilesizeUploadAttribute(const std::vector<std::string> & lineSplit);
		 void locationAttribute(const std::vector<std::string> & lineSplit);

		//initializeStruct.cpp

		 void	initializeServer();
		 void	initializeLocation();

	private:
		int _nbLine;
		bool _inServerBlock;
		bool _argumentUsedServer[5];
		bool _inLocationBlock;
		bool _argumentUsedLocation[8];
		bool _inMethodBlock;
		bool _argumentUsedMethod[3];
		std::string _line;
		std::vector<t_server> _servers;
		t_server _server;
		t_location _location;
};
	
//utils.cpp

bool 							isDigit(const std::string string);
bool 							isValidPath(const std::string string);
bool 							isValidPathDir(const std::string string);
void							trimString(std::string & string, const char *charset);
size_t 							countChar(const std::string string, const char delimiter);
const std::string 				ParsingError(std::string error) throw();
const std::string				intToString(const int number);
const std::vector<std::string>	splitString(const std::string string, const char delimiter);
