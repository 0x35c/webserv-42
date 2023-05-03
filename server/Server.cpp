#include "Request.hpp"
#include <iostream>

void Server::exitWithError(const std::string& errorMessage) {
	std::cerr << errorMessage;
	close(_sockfd);
	std::exit(1);
}

void Server::createServerSocket(void) {
	//create the socket server
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0)
		exitWithError("error: socket failed\n");

	int option = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
		exitWithError("error: setsockopt failed\n");
}

void Server::bindPort(void) {
	//bind to the PORT with the informations above
	if (bind(_sockfd, (sockaddr*)&_sockAddr, _sockAddr_len) < 0)
		exitWithError("error: binding failed\n");
}

void Server::listenForRequest(void) {
	//start to listen to request (= wait)
	if (listen(_sockfd, 1) < 0)
		exitWithError("error: listen failed\n");
	std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: " << inet_ntoa(_sockAddr.sin_addr)
        << " PORT: " << ntohs(_sockAddr.sin_port)
        << " ***\n\n";
	std::cout << ss.str() << std::endl;
}

Server::Server(std::string ipAddr, int port) {
	_ipAddr = ipAddr;
	_port = port;
	_sockAddr_len = sizeof(_sockAddr);
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_port = htons(_port);
	_sockAddr.sin_addr.s_addr = inet_addr(_ipAddr.c_str());

	createServerSocket();
	bindPort();
	listenForRequest();
	
	Request request(*this);

	if (_sockfd > 0)
		request.acceptRequest();
}

Server::Server(void) {
}

Server::Server(const Server& other){
	_sockfd = other._sockfd;
	_sockAddr = other._sockAddr;
	_sockAddr_len = other._sockAddr_len;
}

Server&	Server::operator=(const Server &other){
	(void)other;
	return (*this);
}

Server::~Server(void){
	close(_sockfd);
}
