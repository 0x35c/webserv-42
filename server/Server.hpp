#pragma once

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <csignal>
#include <vector>
#include <exception>
#include <cstring>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <fstream>
#include <utility>
#include <sys/select.h>
#include <vector>

#define LISTEN_BACKLOG 128

class Server {
	public:
		Server(void);
		~Server();

		void start(void);
		void addAddress(std::string const &address, int port);
		void emergencyStop(void);

		class ServerException : public std::exception {
			public:
				char const *what(void) const throw();
		};


	private:
		struct _socket {
			int			fd;
			sockaddr_in	address;
		};

		struct _request {
			int			fd;
			bool		isDone;
		};

		struct _socketFinder {
			int _fd;
			_socketFinder(int fd) : _fd(fd) {}
			bool operator()(_socket const &socket) const {
				return socket.fd == _fd;
			}
		};

		std::vector<_socket>	_sockets;
		socklen_t				_addressLen;
		int						_nbConnections;
		fd_set 					_readSet;

		Server(Server const &other);
		Server &operator=(Server const &other);

		void _readFile(char const *file, std::string &buffer);
		void _acceptConnection(_socket const &sock);
		void _processRequest(int fd);
};
