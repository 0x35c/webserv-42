#include "Request.hpp"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctime>

static char **getEnvpInArray(std::map<std::string, std::string> _cgiEnv);

Request::Request(void)
	: _clientfd(-1), _method("NTM")
{ _cgi.inCGI = false; }

Request::Request(int clientfd, const t_server& serverConfig)
	: _clientfd(clientfd), _method("NTM"), _serverConfig(serverConfig)
{ _cgi.inCGI = false; }

Request::Request(const Request& other)
	: _clientfd(other._clientfd), _method(other._method), _statusCode(other._statusCode),
		_boundary(other._boundary), _query(other._query),
		_requestHeader(other._requestHeader), _isDirectory(other._isDirectory),
		_serverConfig(other._serverConfig)
{ _cgi.inCGI = other._cgi.inCGI; }

Request&	Request::operator=(const Request& other) {
	if (this != &other) {
		_clientfd = other._clientfd;
		_method = other._method;
		_statusCode = other._statusCode;
		_boundary = other._boundary;
		_query = other._query;
		_requestHeader = other._requestHeader;
		_isDirectory = other._isDirectory;
		_serverConfig = other._serverConfig;
		_cgi.inCGI = other._cgi.inCGI;
	}
	return *this;
}

void Request::sendErrorResponse(void) {
	std::ifstream file(_requestHeader[HEAD].c_str());

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: text/html\r\n";
	ss << "Content-Length: " << fileSize << "\r\n\r\n";
	ss << file.rdbuf();
	send(_clientfd, ss.str().c_str(), ss.str().size(), 0);
}

void Request::initializeEnvpCGI(void) {
	_cgiEnv["SERVER_SOFTWARE"] = "BOZOSERVER/2.0";
	_cgiEnv["SERVER_NAME"] = _serverConfig.server_name;
	_cgiEnv["SERVER_PORT"] = _serverConfig.port;
	_cgiEnv["SERVER_PROTOCOL"] = "HTTP/1.1";
	_cgiEnv["REQUEST_METHOD"] = _method;
	_cgiEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
	_cgiEnv["CONTENT_TYPE"] = _requestHeader[CONTENT_TYPE];
	_cgiEnv["CONTENT_LENGTH"] = _requestHeader[CONTENT_LENGTH];
	_cgiEnv["REMOTE_ADDR"] = "0.0.0.0";
	_cgiEnv["PATH"] = _location->root;
	_cgiEnv["HTTP_USER_AGENT"] = _requestHeader[USER_AGENT];
}

static char **getEnvpInArray(std::map<std::string, std::string> _cgiEnv) {
	char **arrayEnvpVariable =  new char *[_cgiEnv.size() + 1];
	int i = 0;
	std::string *tmp;
	std::map<std::string, std::string>::iterator it;
	for (it = _cgiEnv.begin(); it != _cgiEnv.end(); it++) {
		tmp = new std::string;
		*tmp = it->first + "=" + it->second + "\0";
		arrayEnvpVariable[i] = (char *)tmp->c_str();
		i++;
	}
	arrayEnvpVariable[i] = NULL;
	return (arrayEnvpVariable);
}

void Request::executeCGI(std::string fileName) {
	initializeEnvpCGI();
	_cgiEnv["SCRIPT_NAME"] = fileName;
	_cgiEnv["URL"] = fileName;
	if (pipe(_cgi.fds[0]) == -1
		|| pipe(_cgi.fds[1]) == -1)
		throw (Request::RequestException());
	std::cout << "_query:" + _query +"\n";
	int pid = fork();
	if (pid == 0)
	{
		if (dup2(_cgi.fds[0][0], 0) == -1
			|| dup2(_cgi.fds[1][1], 1) == -1
			|| close(_cgi.fds[0][0]) == -1
			|| close(_cgi.fds[0][1]) == -1
			|| close(_cgi.fds[1][0]) == -1
			|| close(_cgi.fds[1][1]) == -1)
			throw (Request::RequestException());

		/* if (_method == "POST") */
		/* 	_query = _requestHeader[BODY]; */
		_cgiEnv["QUERY_STRING"] = _query;
		char *args[4] = {(char *)"/usr/bin/python3", (char *)(fileName.c_str()),
						(char *)(_query.c_str()),NULL};
		char **EnvpVariables = getEnvpInArray(_cgiEnv);
		execve("/usr/bin/python3", args, EnvpVariables);
		for (size_t i = 0; EnvpVariables[i]; i++)
			delete EnvpVariables[i];
		delete [] EnvpVariables;
		exit(EXIT_FAILURE);
	}
	else if (pid != 0)
	{
		if (close(_cgi.fds[0][0]) == -1
			|| close(_cgi.fds[0][1]) == -1
			|| close(_cgi.fds[1][1]) == -1)
			throw (Request::RequestException());
		if (pid > 0)
		{
			_cgi.pid = pid;
			_cgi.begin_time = std::time(NULL);
		}
		else if (pid < 0)
		{
			_cgi.inCGI = false;
			if (close(_cgi.fds[1][0]) == -1)
				throw (Request::RequestException());
		}
	}
}

void Request::respondToGetRequest(void) {
	DIR* directory = opendir(_requestHeader[HEAD].c_str());
	_isDirectory = false;
	_cgi.inCGI = false;
	if (directory == NULL) {
		std::string fileName = _requestHeader[HEAD];
		if (fileName.length() > 3 && fileName.substr(fileName.length() - 3) == ".py") {
			_cgi.inCGI = true;
			if (setStatusCode() == 400)
				return (sendErrorResponse());
			executeCGI(fileName);
		}
		else {
			if (setStatusCode() == 400)
				return (sendErrorResponse());
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
			ss.str("");
			ss.clear();
			file.close();
		}
	}
	else {
		_isDirectory = true;
		if (setStatusCode() == 400)
			return (sendErrorResponse());
		directoryListing(directory, _requestHeader[HEAD]);
	}
}

void Request::respondToPostRequest(void) {
	int statusCode = setStatusCode();
	if (statusCode == 400)
		return (sendErrorResponse());

	_cgi.inCGI = false;
	std::string fileName = _requestHeader[HEAD];
	if (fileName.length() > 3 && fileName.substr(fileName.length() - 3) == ".py") {
		_cgi.inCGI = true;
		if (setStatusCode() == 400)
			return (sendErrorResponse());
		executeCGI(fileName);
		return ;
	}
	std::ostringstream ss;
	if (_requestHeader[HEAD] != "") {
		_requestHeader[LOCATION] = "/done.html";
	}
	else
		_requestHeader[LOCATION] = "/";
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " << _requestHeader[ACCEPT] << "\r\n";
	if (_statusCode == "302 Redirect")
		ss << "Location: " << _requestHeader[LOCATION] << "\r\n";
	ss << "Content-length: 0" << "\r\n\r\n";
	send(_clientfd, ss.str().c_str(), ss.str().size(), 0);

	if (statusCode == 200) {
		std::ofstream file((_location->uploadedFilePath + _requestHeader[HEAD]).c_str());
		file << _requestHeader[BODY];
		file.close();
	}
}

void Request::respondToDeleteRequest(void) {
	setStatusCode();
	std::string fileName = _requestHeader[HEAD];
	std::cout << fileName << std::endl;
	std::remove(fileName.c_str());
	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " << _requestHeader[ACCEPT] + "\r\n";
	ss << "Content-Length: 0\r\n\r\n";
	send(_clientfd, ss.str().c_str(), ss.str().size(), 0);
}

const std::string Request::getMethod(std::string buffer) {
	if (buffer.find("GET") != std::string::npos)
		return ("GET");
	else if (buffer.find("POST") != std::string::npos)
		return ("POST");
	else if (buffer.find("DELETE") != std::string::npos)
		return ("DELETE");
	return ("FORBIDDEN");
}

bool Request::readRequest(std::string const &rawRequest) {
	static bool headerRead = false;
	if (headerRead == false) {
		_method = getMethod(rawRequest);
		parseHeader(rawRequest);
		if (_method == "POST" && _boundary.empty())
		{
			headerRead = false;
			return (true);
		}
		std::string tmpHeader = rawRequest.substr(0, rawRequest.find("\r\n\r\n"));
		std::string tmpBodyHeader = rawRequest.substr(rawRequest.find("\r\n\r\n") + 4, std::string::npos);
		tmpBodyHeader = tmpBodyHeader.substr(0, tmpBodyHeader.find("\r\n\r\n"));
		std::string tmpBoundary = tmpBodyHeader.substr(0, tmpBodyHeader.find("\r"));
		if (!(_requestHeader[TRANSFER_ENCODING] == "chunked" && _method == "POST")) {
			long contentLength = std::atol(_requestHeader[CONTENT_LENGTH].c_str()) + tmpHeader.length() + 4 + tmpBodyHeader.length() + tmpBoundary.length() + 2;
			if (_method == "GET" || _method == "DELETE" || (_method == "POST" && contentLength < BUFFER_SIZE)) {
				headerRead = false;
				return (true);
			}
		}
		headerRead = true;
	}
	else {
		if (parseBody(rawRequest) == true) {
			headerRead = false;
			return (true);
		}
	}
	return (false);
}

Request::~Request(void) {
}

int Request::getClientfd(void) const {
	return (_clientfd);
}

t_cgi& Request::getCGI(void) {
	return (_cgi);
}

const std::string& Request::getStatusCode(void) const {
	return (_statusCode);
}

char const *Request::RequestException::what(void) const throw() {
	return strerror(errno);
}
