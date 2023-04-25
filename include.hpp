#ifndef INCLUDE_HPP
#define INCLUDE_HPP

#include <sstream>
#include <fstream>
#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <string>

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

#define PORT 8080
#define IP "0.0.0.0"

#endif
