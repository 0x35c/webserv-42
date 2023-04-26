#include "Server.hpp"
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
    ss << "\n*** Listening on ADDRESS: "
        << inet_ntoa(_sockAddr.sin_addr)
        << " PORT: " << ntohs(_sockAddr.sin_port)
        << " ***\n\n";
	std::cout << ss.str() << std::endl;
}

void Server::respondToRequest(void) {
	std::ifstream file(_path.c_str(), std::ios::in | std::ios::binary);

	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize <= 0)
		exitWithError("error: empty file\n");
	const char* data = new char[fileSize];

	file.read((char *)data, fileSize);
	file.close();

	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\r\n";
	ss << "Content-type: " + _type + "\r\n";
	ss << "Content-Length: " << fileSize << "\r\n\r\n";
	ss.write(data, fileSize);
	write(_clientfd, ss.str().c_str(), ss.str().size());
	delete [] data;
}

void Server::readRequest(void) {
	char buffer[30720] = {0};
	if (read(_clientfd, buffer, 30720 - 1) < 0)
		exitWithError("error: failed to read client socket\n");
	std::cout << buffer << "\n";
	if (strncmp(buffer, "GET /ulayus.jpg HTTP/1.1", 24) == 0) {
		_type = "image/avif";
		_path = "server/ulayus.jpg";
	}
	else if (strncmp(buffer, "GET /images.png HTTP/1.1", 24) == 0) {
		_type = "image/avif";
		_path = "server/images.png";
	}
	else if (strncmp(buffer, "GET /style.css HTTP/1.1", 23) == 0) {
		_type = "text/css";
		_path = "server/style.css";
	}
	else {
		_type = "text/html";
		_path = "server/index.html";
	}
	respondToRequest();
}

void Server::acceptRequest(void) {
	while (true)
	{
		//when someone connect, it accept it
		_clientfd = accept(_sockfd, (sockaddr*)&_sockAddr, &_sockAddr_len);
		if (_clientfd < 0) {
			std::ostringstream ss;
			ss <<
			"Server failed to accept incoming connection from ADDRESS: "
			<< inet_ntoa(_sockAddr.sin_addr) << "; PORT: "
			<< ntohs(_sockAddr.sin_port);
			exitWithError("error: log\n");
		}
		std::cout << "someone connected.\n";
		readRequest();
		close(_clientfd);
	}
}

Server::Server(std::string ipAddr, int port){
	_ipAddr = ipAddr;
	_port = port;
	_sockAddr_len = sizeof(_sockAddr);
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_port = htons(_port);
	_sockAddr.sin_addr.s_addr = inet_addr(_ipAddr.c_str());
	
	createServerSocket();
	bindPort();
	listenForRequest();
	if (_sockfd > 0)
		acceptRequest();
}

Server::Server(const Server& other){
	(void)other;
}

Server&	Server::operator=(const Server &other){
	(void)other;
	return (*this);
}

Server::~Server(void){
}
