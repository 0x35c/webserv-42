#pragma once

#include <string>
#include <map>
#include <dirent.h>
#include <sys/socket.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "parsing/parsing.hpp"

#define BUFFER_SIZE 8192
#define DEBUG 1
typedef std::map<int, std::string> strMap;
typedef std::pair<int, std::string> strPair;

enum methods {
	GET,
	POST,
	DELETE
};

enum headerAttributes {
	BODY,
	HEAD,
	HOST,
	USER_AGENT,
	ACCEPT,
	LOCATION,
	ACCEPT_LANGUAGE,
	ACCEPT_ENCODING,
	CONTENT_TYPE,
	BOUNDARY,
	CONTENT_LENGTH,
	ORIGIN,
	CONNECTION,
	REFERER,
	UPGRADE_INSECURE_REQUESTS,
	SEC_FETCH_DEST,
	SEC_FETCH_MODE,
	SEC_FETCH_SITE,
	SEC_FETCH_USER,
	PRAGMA,
	CACHE_CONTROL
};

typedef struct s_cgi {
	bool	inCGI;
	int		pid;
	int		fds[2][2];
	time_t	begin_time;
} t_cgi;

class Request {
	public:
		Request(void);
		Request(int clientfd, const t_server& serverConfig);
		Request(const Request& other);
		Request& operator=(const Request& other);
		~Request(void);

		// Public member functions
		bool readRequest(std::string const& rawRequest);
		void respondToRequest(void);

		int getClientfd(void) const;
		t_cgi& getCGI(void);

	// Private member functions
	private:
		void respondToGetRequest(void);
		void respondToGetCGI(std::string fileName);
		void respondToPostRequest(void);
		void respondToPostCGI(std::string fileName);
		void respondToDeleteRequest(void);
		void errorOnRequest(void);
		void processLine(std::string line, int lineToken);
		void parseHeader(const std::string& request);
		bool parseBody(const std::string& buffer);
		int setStatusCode(void);
		int getLineToken(std::string line);
		const std::string getMethod(std::string buffer);
		void directoryListing(DIR* directory, const std::string& dirName);
		void initializeEnvpCGI(void);

	// Private member attributes
	private:
		int	_clientfd;
		std::string  _method;
		std::string _statusCode;
		std::string _boundary;
		std::string _query;
		strMap _requestHeader;
		bool _isDirectory;
		bool _validRequest;
		std::map<std::string, std::string> _cgiEnv;
		t_cgi _cgi;
		t_server _serverConfig;
		t_location* _location;
};
