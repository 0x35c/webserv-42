#ifndef INCLUDE_HPP
#define INCLUDE_HPP

#include <sstream>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <limits>
#include <map>

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
/*
struct in_addr{
	unsigned long s_addr;
};

struct sockaddr_in {
	short sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};
*/
typedef std::map<int, std::string> strMap;
typedef std::pair<int, std::string> strPair;

enum methods {
	ERROR,
	GET,
	POST,
	DELETE
};

enum attributes {
	HEAD,
	HOST,
	USER_AGENT,
	ACCEPT,
	ACCEPT_LANGUAGE,
	ACCEPT_ENCODING,
	CONTENT_TYPE,
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
	CACHE_CONTROL,
	BODY = -1
};

#define PORT 8080
#define IP "0.0.0.0"
#define IMG 1
#define HTML 2
#define BUFFER_SIZE 2 << 18

#endif
