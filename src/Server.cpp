#include <cstdlib>
#include <algorithm>

#include "Server.hpp"
#include "Request.hpp"

Server::Server(void)
	: _addressLen(sizeof(sockaddr_in)), _nbConnections(0)
{
	FD_ZERO(&_readSet);
}

void Server::addAddress(std::string const &address, int port)
{
	struct _socket newSocket;
	newSocket.address.sin_family = AF_INET;
	newSocket.address.sin_port = htons(port);
	newSocket.address.sin_addr.s_addr = inet_addr(address.c_str());

	newSocket.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (newSocket.fd < 0)
		throw ServerException();

	int option = 1;
	if (setsockopt(newSocket.fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
		throw ServerException();

	if (bind(newSocket.fd, (sockaddr*)&newSocket.address, _addressLen) < 0)
		throw ServerException();

	if (listen(newSocket.fd, LISTEN_BACKLOG) < 0)
		throw ServerException();

	FD_SET(newSocket.fd, &_readSet);
	_sockets.push_back(newSocket);
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
	for (std::vector<_socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
		close(it->fd);
}

void Server::emergencyStop(void)
{
	for (std::vector<_socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
		close(it->fd);
	std::cout << "server stopped\n";
}

void Server::start(void)
{
	fd_set readSet;

	for (std::vector<_socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
	{
		std::cout << "listening on " << inet_ntoa(it->address.sin_addr) << ":" << ntohs(it->address.sin_port) << "\n";
	}
	while (true)
	{
		readSet = _readSet;

		if (select(FD_SETSIZE + 1, &readSet, NULL, NULL, NULL) < 0)
			throw ServerException();

		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &readSet))
			{
				std::vector<_socket>::iterator socket = std::find_if(_sockets.begin(), _sockets.end(), _socketFinder(i));
				if (socket != _sockets.end())
					_acceptConnection(*socket);
				else
					_processRequest(i);
			}
		}
	}
}

void Server::_readFile(char const *filePath, std::string &buffer)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
		throw ServerException();

	file.seekg(0, file.end);
	std::size_t size = file.tellg();
	file.seekg(0, file.beg);

	buffer.resize(size);
	file.read(&buffer[0], size);

	file.close();
}

void Server::_acceptConnection(_socket const &sock)
{
	int clientSocket = accept(sock.fd, (sockaddr*)&sock.address, &_addressLen);
	if (clientSocket < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw ServerException();
		return ;
	}

	FD_SET(clientSocket, &_readSet);
	++_nbConnections;
	std::cout << "accepted connection\n";
}

void Server::_processRequest(int fd)
{
	std::string header_buffer(1024, 0);
	int rc = recv(fd, &header_buffer[0], 1024, 0);
	if (rc <= 0)
	{
		close(fd);
		FD_CLR(fd, &_readSet);
		--_nbConnections;
		if (rc < 0)
			std::cerr << "error: " << strerror(errno) << "\n";
		std::cout << "closed connection\n";
		return ;
	}
	std::cout << rc << " bytes read\n";

	Request request(fd);
	request.readRequest(header_buffer);

	std::cout << "sent response\n";
}

char const *Server::ServerException::what(void) const throw() {
	return strerror(errno);
}
