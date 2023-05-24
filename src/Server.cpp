#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <sys/wait.h>
#include "Server.hpp"

Server::Server(void)
	: _addressLen(sizeof(sockaddr_in))
{
	FD_ZERO(&_readSet);
}

void Server::addAddress(const t_server& serverConfig)
{
	sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(serverConfig.port);
	socketAddress.sin_addr.s_addr = inet_addr(serverConfig.host.c_str());

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw ServerException();
	int option = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
		throw ServerException();

	if (bind(fd, (sockaddr*)&socketAddress, _addressLen) < 0)
		throw ServerException();

	if (listen(fd, LISTEN_BACKLOG) < 0)
		throw ServerException();

	FD_SET(fd, &_readSet);
	_sockets[fd] = serverConfig;
	_sockets[fd].socketAddress = socketAddress;
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
	for (requestMap::iterator it = _requests.begin(); it != _requests.end(); )
	{
		if (it->second.getCGI().inCGI == true)
		{
			close(it->second.getCGI().fds[1][0]);
			kill(it->second.getCGI().pid, SIGKILL);
			/*
			if (close(it->second.getCGI().fds[1][0]) == -1)
				throw (ServerException());
			if (kill(it->second.getCGI().pid, SIGKILL) < 0)
				throw (ServerException());
			*/
		}
		it++;
	}
	for (socketMap::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
		close(it->first);
	for (requestMap::iterator it = _requests.begin(); it != _requests.end(); ++it)
		close(it->first);
}

void Server::checkCGI(void)
{
	for (requestMap::iterator it = _requests.begin(); it != _requests.end(); )
	{
		if (it->second.getCGI().inCGI == true)
		{
			std::ostringstream ss;
			bool stopCGI = false;
			if (std::time(NULL) - it->second.getCGI().begin_time > TIMEOUT_CGI)
			{
				stopCGI = true;
				if (kill(it->second.getCGI().pid, SIGKILL) < 0)
					throw (ServerException());
				std::string InfiniteLoopHTML = "<html><body><h1>Infinite loop in CGI</h1></body></html>";
				ss << "HTTP/1.1 508 Loop Detected\r\n";
				ss << "Content-type: text/html\r\n";
				ss << "Content-Length: " << InfiniteLoopHTML.size() << "\r\n\r\n";
				ss << InfiniteLoopHTML;
			}
			else if (waitpid(it->second.getCGI().pid, NULL, WNOHANG) > 0)
			{
				stopCGI = true;
				char buffer[BUFFER_SIZE] = {0};
				int fileSize = read(it->second.getCGI().fds[1][0], buffer, BUFFER_SIZE);
				if (fileSize < 0)
					throw (ServerException());
				ss << "HTTP/1.1 200\r\n";
				ss << "Content-type: text/html\r\n";
				ss << "Content-Length: " << fileSize << "\r\n\r\n";
				ss << buffer;
			}
			if (stopCGI)
			{
				if (close(it->second.getCGI().fds[1][0]) == -1)
					throw (ServerException());
				it->second.getCGI().inCGI = false;
				if (write(it->second.getClientfd(), ss.str().c_str(), ss.str().size()) < 0)
					throw (ServerException());
				ss.str("");
				ss.clear();
			}
		}
		it++;
	}
}

void Server::start(void)
{
	fd_set readSet;

	for (socketMap::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
		std::cout << "Listening on " << inet_ntoa(it->second.socketAddress.sin_addr) << ":" << ntohs(it->second.socketAddress.sin_port) << std::endl;
	while (true)
	{
		readSet = _readSet;
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;
		int rv = select(FD_SETSIZE + 1, &readSet, NULL, NULL, &timeout);
		if (rv < 0)
			throw ServerException();
		checkCGI();
		if (rv == 0)
			continue ;
		for (socketMap::iterator it = _sockets.begin(); it != _sockets.end(); it++) 
			if (FD_ISSET(it->first, &readSet))
				_acceptConnection(it->first, it->second);

		for (requestMap::iterator it = _requests.begin(); it != _requests.end(); )
		{
			if (FD_ISSET(it->first, &readSet) && it->second.getCGI().inCGI == false)
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

void Server::_acceptConnection(int socketFd, const t_server& serverConfig)
{
	int fd = accept(socketFd, (sockaddr *)&(serverConfig.socketAddress), &_addressLen);
	if (fd < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw ServerException();
		return ;
	}
	FD_SET(fd, &_readSet);
	_requests[fd] = Request(fd, serverConfig);
	_requests[fd].getCGI().inCGI = false;
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
