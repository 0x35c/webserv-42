#include "Request.hpp"
#include <cstdlib>
#include <iostream>

Request::Request(void)
	: _clientfd(-1), _method(-1)
{}

Request::Request(int clientfd)
	: _clientfd(clientfd), _method(-1) {}

Request::Request(const Request& other)
	: _clientfd(other._clientfd), _method(other._method), _statusCode(other._statusCode),
		_boundary(other._boundary), _query(other._query),
		_requestHeader(other._requestHeader), _isDirectory(other._isDirectory)
{}

Request&	Request::operator=(const Request& other) {
	if (this != &other) {
		_clientfd = other._clientfd;
		_method = other._method;
		_statusCode = other._statusCode;
		_boundary = other._boundary;
		_query = other._query;
		_requestHeader = other._requestHeader;
		_isDirectory = other._isDirectory;
	}
	return *this;
}

static void editName(std::string& name) {
	while (true) {
		char* name_c = (char *)name.c_str();
		size_t len;
		if (name.find("%") != std::string::npos) {
			for (size_t i = 0; name_c[i]; i++) {
				if (name_c[i] == '%') {
					name_c = &(name_c[i + 1]);
					break ;
				}
			}
			long c = std::strtol(name_c, NULL, 16);
			for (len = 0; std::isdigit(name_c[len]); len++);
			name.replace(name.find("%"), len + 1, (const char*)&c);
		}
		else if (name.find("+") != std::string::npos) {
			name.replace(name.find("+"), 1, " ");
		}
		else
			break ;
	}
}

static void getQuery(std::string& query, const std::string& name) {
	size_t delimiter = name.find("?");
	if (delimiter != std::string::npos)
		query = name.substr(delimiter + 1);
}

void Request::respondToGetRequest(void) {
	editName(_requestHeader[HEAD]);
	getQuery(_query, _requestHeader[HEAD]);
	std::cout << _query << std::endl;
	DIR* directory = opendir(_requestHeader[HEAD].c_str());
	_isDirectory = false;
	if (directory == NULL) {
		setStatusCode();
		std::ifstream file(_requestHeader[HEAD].c_str(), std::ios::in | std::ios::binary);

		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		std::ostringstream ss;
		ss << "HTTP/1.1 " << _statusCode << "\r\n";
		ss << "Content-type: " << _requestHeader[ACCEPT] << "\r\n";
		ss << "Content-Length: " << fileSize << "\r\n\r\n";
		ss << file.rdbuf();

		send(_clientfd, ss.str().c_str(), ss.str().size(), 0);
		file.close();
	}
	else {
		_isDirectory = true;
		setStatusCode();
		directoryListing(directory, _requestHeader[HEAD]);
	}
}

void Request::respondToPostRequest(void) {
	setStatusCode();
	std::ofstream file(("www/uploads/" + _requestHeader[HEAD]).c_str());

	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " << _requestHeader[ACCEPT] << "\r\n";
	ss << "Location: www/uploads/" << _requestHeader[HEAD] << "\r\n\r\n";
	send(_clientfd, ss.str().c_str(), ss.str().size(), 0);

	/* std::cout << _requestHeader[BODY]; */
	/* std::string buffer(BUFFER_SIZE, 0); */
	/* size_t length = 0; */
	/* while (length < _requestHeader[BODY].length()) { */
	/* 	if (length + BUFFER_SIZE < _requestHeader[BODY].length()) { */
	/* 		buffer = _requestHeader[BODY].substr(length, length + BUFFER_SIZE); */
	/* 		length += BUFFER_SIZE; */
	/* 	} */
	/* 	else */
	/* 		buffer = _requestHeader[BODY].substr(length, _requestHeader[BODY].length()); */
	/* 	file << buffer; */
	/* 	buffer.clear(); */
	/* } */

	file << _requestHeader[BODY];
	/* file.write(_requestHeader[BODY].c_str(), _requestHeader[BODY].length()); */
	file.close();
}

void Request::respondToDeleteRequest(void) {
	setStatusCode();
	std::string body = _requestHeader[BODY];
	std::fstream file(_requestHeader[HEAD].c_str(), std::fstream::out);

	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " + _requestHeader[ACCEPT] + "\r\n";
	ss << "Content-Length: " << _requestHeader[CONTENT_LENGTH] << "\r\n\r\n";
	send(_clientfd, ss.str().c_str(), ss.str().size(), 0);
	file << body;
	file.close();
}

static int getMethod(std::string buffer) {
	if (buffer.find("GET") != std::string::npos)
		return (GET);
	else if (buffer.find("POST") != std::string::npos)
		return (POST);
	else if (buffer.find("DELETE") != std::string::npos)
		return (DELETE);
	return (ERROR);
}

bool Request::readRequest(std::string const &rawRequest) {
	static bool headerRead = false;
	if (headerRead == false) {
		_method = getMethod(rawRequest);
		parseHeader(rawRequest);
		if (_method == GET)
			return (true);
		headerRead = true;
	}
	else
		return (parseBody(rawRequest));
	return (false);
}

Request::~Request(void){
}

int Request::getClientfd(void) const {
	return _clientfd;
}
