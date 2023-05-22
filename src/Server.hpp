#pragma once

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <csignal>
#include <exception>
#include <cstring>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <fstream>
#include <utility>
#include <sys/select.h>
#include <map>

#include "Request.hpp"

#define LISTEN_BACKLOG 128
#define DEBUG 0

class Server {
	public:
		Server();
		~Server();

		void start(void);
		void addAddress(std::string const &address, int port);

		class ServerException : public std::exception {
			public:
				char const *what(void) const throw();
		};


	private:
		typedef std::map<int, sockaddr_in> socketMap;
		typedef std::map<int, Request> requestMap;

		socketMap	_sockets;
		requestMap	_requests;
		socklen_t	_addressLen;
		fd_set 		_readSet;

		Server(Server const &other);
		Server &operator=(Server const &other);

		void _acceptConnection(int socketFd, sockaddr_in *address);
		bool _processRequest(int clientFd, Request &request);
};
