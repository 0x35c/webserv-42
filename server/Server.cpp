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
    ss << "\n*** Listening on ADDRESS: " << inet_ntoa(_sockAddr.sin_addr)
        << " PORT: " << ntohs(_sockAddr.sin_port)
        << " ***\n\n";
	std::cout << ss.str() << std::endl;
}

void Server::respondToGetRequest(void) {
	bool canSend = setStatusCode();
	std::ifstream file(_requestHeader[HEAD].c_str(), std::ios::in | std::ios::binary);

	file.seekg(0, std::ios::end);
	long fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (canSend == false)
		fileSize = 0;
	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " + _requestHeader[ACCEPT] + "\r\n";
	ss << "Content-Length: " << fileSize << "\r\n\r\n";
	write(_clientfd, ss.str().c_str(), ss.str().size());
	ss.str("");
	ss.clear();

	while (!file.eof() && canSend) {
		if (fileSize > BUFFER_SIZE) {
			file.read((char *)_buffer, BUFFER_SIZE);
			ss.write(_buffer, BUFFER_SIZE);
			fileSize -= BUFFER_SIZE;
		}
		else {
			file.read((char *)_buffer, fileSize);
			ss.write(_buffer, fileSize);
		}
		write(_clientfd, ss.str().c_str(), ss.str().size());
		ss.str("");
		ss.clear();
	}
	file.close();
}

void Server::respondToPostRequest(void) {
	setStatusCode();
	std::string body = _requestHeader[BODY];
	std::fstream file(_requestHeader[HEAD].c_str(), std::fstream::out);

	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " + _requestHeader[ACCEPT] + "\r\n";
	ss << "Content-Length: " << _requestHeader[CONTENT_LENGTH] << "\r\n\r\n";
	write(_clientfd, ss.str().c_str(), ss.str().size());
	file << body;
	file.close();
}

void Server::respondToDeleteRequest(void) {
	setStatusCode();
	std::string body = _requestHeader[BODY];
	std::fstream file(_requestHeader[HEAD].c_str(), std::fstream::out);

	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " + _requestHeader[ACCEPT] + "\r\n";
	ss << "Content-Length: " << _requestHeader[CONTENT_LENGTH] << "\r\n\r\n";
	write(_clientfd, ss.str().c_str(), ss.str().size());
	file << body;
	file.close();
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
		readRequest();
		close(_clientfd);
	}
}

Server::Server(std::string ipAddr, int port): _buffer(new char[BUFFER_SIZE]) {
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
	delete [] _buffer;
	close(_clientfd);
	close(_sockfd);
}
