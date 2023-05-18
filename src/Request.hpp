#pragma once

#include <string>
#include <map>
#include <dirent.h>
#include <sys/socket.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstdlib>

typedef std::map<int, std::string> strMap;
typedef std::pair<int, std::string> strPair;

#define BUFFER_SIZE 4096

enum methods {
	ERROR,
	GET,
	POST,
	DELETE
};

enum attributes {
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

class Request {
	public:
		Request(void);
		Request(int clientfd);
		Request(const Request& other);
		Request& operator=(const Request& other);
		~Request(void);

		// Public member functions
		bool readRequest(std::string const &rawRequest);
		void respondToRequest(void);

		int getClientfd(void) const;

	// Private member functions
	private:
		void respondToGetRequest(void);
		void respondToPostRequest(void);
		void respondToDeleteRequest(void);
		void errorOnRequest(void);
		void processLine(std::string line, int lineToken);
		void parseHeader(const std::string& request);
		bool parseBody(const std::string& buffer);
		bool setStatusCode(void);
		void directoryListing(DIR* directory, const std::string& dirName);

	// Private member attributes
	private:
		int	_clientfd;
		int _method;
		std::string _statusCode;
		std::string _boundary;
		std::string _query;
		strMap _requestHeader;
		bool _isDirectory;
};
