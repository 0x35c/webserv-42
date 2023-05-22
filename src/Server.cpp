#include <cstdlib>
#include <algorithm>
#include <fcntl.h>

#include "Server.hpp"

Server::Server(void)
	: _addressLen(sizeof(sockaddr_in))
{
	FD_ZERO(&_readSet);
}

void Server::addAddress(std::string const &address, int port)
{
	sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(port);
	socketAddress.sin_addr.s_addr = inet_addr(address.c_str());

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw ServerException();
	
	/*
	int flag = fcntl(fd, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flag);
	*/

	int option = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
		throw ServerException();

	if (bind(fd, (sockaddr*)&socketAddress, _addressLen) < 0)
		throw ServerException();

	if (listen(fd, LISTEN_BACKLOG) < 0)
		throw ServerException();

	FD_SET(fd, &_readSet);
	_sockets[fd] = socketAddress;
}

Server::Server(Server const &other)
{
	(void)other;
}

Server &Server::operator=(Server const &other)
{
	(void)other;
	return *this;
}

Server::~Server()
{
	for (socketMap::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
		close(it->first);
	for (requestMap::iterator it = _requests.begin(); it != _requests.end(); ++it)
		close(it->first);
}

void Server::start(void)
{
	fd_set readSet;

	for (socketMap::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
		std::cout << "Listening on " << inet_ntoa(it->second.sin_addr) << ":" << ntohs(it->second.sin_port) << std::endl;
	while (true)
	{
		readSet = _readSet;
		if (select(FD_SETSIZE + 1, &readSet, NULL, NULL, NULL) < 0)
			throw ServerException();
		for (socketMap::iterator it = _sockets.begin(); it != _sockets.end(); it++) 
			if (FD_ISSET(it->first, &readSet))
				_acceptConnection(it->first, &it->second);

		for (requestMap::iterator it = _requests.begin(); it != _requests.end(); )
		{
			if (FD_ISSET(it->first, &readSet))
			{
				if (_processRequest(it->first, it->second))
				{
					_requests.erase(it++);
					continue ;
				}
			}
			it++;
		}
	}
}

void Server::_acceptConnection(int socketFd, sockaddr_in *address)
{
	int fd = accept(socketFd, (sockaddr *)address, &_addressLen);
	if (fd < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw ServerException();
		return ;
	}

	FD_SET(fd, &_readSet);
	_requests[fd] = Request(fd);
#if DEBUG
	std::cout << "accepted connection\n";
#endif
}

bool Server::_processRequest(int clientFd, Request &request)
{
	std::string header_buffer(BUFFER_SIZE, 0);
	int rc = recv(clientFd, &header_buffer[0], BUFFER_SIZE, 0);
	if (rc <= 0)
	{
		close(clientFd);
		FD_CLR(clientFd, &_readSet);
		if (rc < 0)
			std::cerr << "error: " << strerror(errno) << "\n";
#if DEBUG
		std::cout << "closed connection\n";
#endif
		return true;
	}
#if DEBUG
	std::cout << rc << " bytes read\n";
#endif
	if (request.readRequest(header_buffer))
	{
		request.respondToRequest();
#if DEBUG
		std::cout << "response sent\n";
#endif
	}

	return false;
}

char const *Server::ServerException::what(void) const throw() {
	return strerror(errno);
}
