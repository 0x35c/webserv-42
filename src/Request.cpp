#include "Request.hpp"
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctime>

extern char ** g_env;
Request::Request(void)
	: _clientfd(-1), _method(-1)
{}

Request::Request(int clientfd)
	: _clientfd(clientfd), _method(-1)
{}

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
	std::string subName = name;
	while (true) {
		size_t pos = subName.find("%");
		if (pos != std::string::npos) {
			std::string tmp = subName.substr(pos + 1, std::string::npos);
			char* name_c = (char *)tmp.c_str();
			name_c[2] = '\0';
			long c = std::strtol(name_c, NULL, 16);
			name.replace(name.find("%"), 3, (const char*)&c);
			subName = tmp;
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

void Request::initializeEnvpCGI(void)
{
}	

char **Request::GetEnvpInArray(void)
{
	char **arrayEnvpVariable =  new char *[this->_envpVariable.size()];
	int i = 0;
	std::string tmp;
	std::map<std::string, std::string>::iterator it;
	for (it = this->_envpVariable.begin(); it != this->_envpVariable.end(); it++)
	{
		tmp = it->first + "=" + it->second;
		arrayEnvpVariable[i] = (char *)tmp.c_str();
		i++;
	}

	return (arrayEnvpVariable);
}	

void Request::respondToGetCGI(std::string fileName)
{
	/* TO-DO LIST
	 * need to save the pid to kill it if we do a ctrl C during the loading of the CGI
	 * change the envp variables of execve to the good one
	 * check return of functions (pipe, dup2, close, read, send, kill)
	*/
	int fds[2][2];
	pipe(fds[0]);
	pipe(fds[1]);
	int pid = fork();
	if (pid == 0)
	{
		dup2(fds[0][0], 0);
		dup2(fds[1][1], 1);
		close(fds[0][0]);
		close(fds[0][1]);
		close(fds[1][0]);
		close(fds[1][1]);

		std::string querys = _requestHeader[HEAD].substr(_requestHeader[HEAD].find("?") + 1, std::string::npos);
		char *args[4] = {(char *)"/usr/bin/python3", (char *)(fileName.c_str()),
						(char *)(querys.c_str()),NULL};
		//execve("/usr/bin/python3", args, GetEnvpInArray());
		execve("/usr/bin/python3", args, g_env);
		exit(0);
	}
	else if (pid > 0)
	{
		bool forkFinishedProperly = false;
		std::time_t begin_time = std::time(NULL);
		std::time_t actual_time = std::time(NULL);
		while ((actual_time - begin_time) < TIMEOUT_CGI)
		{
			if (waitpid(pid, NULL, WNOHANG) != 0)
			{
				forkFinishedProperly = true;
				break ;
			}
			actual_time = std::time(NULL);
		}
		close(fds[0][0]);
		close(fds[0][1]);
		close(fds[1][1]);
		std::ostringstream ss;
		setStatusCode();
		if (forkFinishedProperly)
		{
			char buffer[BUFFER_SIZE] = {0};
			int fileSize = read(fds[1][0], buffer, BUFFER_SIZE);

			ss << "HTTP/1.1 200\r\n";
			ss << "Content-type: text/html\r\n";
			ss << "Content-Length: " << fileSize << "\r\n\r\n";
			ss << buffer;
		}
		else
		{
			kill(pid, SIGKILL);	
			std::string InfiniteLoopHTML = "<html><body><h1>Infinite loop in CGI</h1></body></html>";
			ss << "HTTP/1.1 200\r\n";
			ss << "Content-type: text/html\r\n";
			ss << "Content-Length: " << InfiniteLoopHTML.size() << "\r\n\r\n";
			ss << InfiniteLoopHTML;
		}
		close(fds[1][0]);
		send(_clientfd, ss.str().c_str(), ss.str().size(), 0);
		ss.str("");
		ss.clear();
	}
}

void Request::respondToGetRequest(void) {
	editName(_requestHeader[HEAD]);
	getQuery(_query, _requestHeader[HEAD]);
#if DEBUG
	std::cout << _query << std::endl;
#endif
	DIR* directory = opendir(_requestHeader[HEAD].c_str());
	_isDirectory = false;
	if (directory == NULL) {
		std::string fileName = _requestHeader[HEAD].substr(0, _requestHeader[HEAD].find("?"));
		if (fileName.substr(fileName.length() - 3) == ".py")
			respondToGetCGI(fileName);
		else
		{
			setStatusCode();
			std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary);

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
		setStatusCode();
		directoryListing(directory, _requestHeader[HEAD]);
	}
}

void Request::respondToPostCGI(std::string fileName)
{
	(void)fileName;
}

void Request::respondToPostRequest(void) {
	int statusCode = setStatusCode();

	std::ostringstream ss;
	if (_requestHeader[HEAD] != "")
		_statusCode = "302 Redirect";
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " << _requestHeader[ACCEPT] << "\r\n";
	if (_statusCode == "302 Redirect")
		ss << "Location: /done.html" << "\r\n";
	ss << "Content-length: 0" << "\r\n\r\n";
	send(_clientfd, ss.str().c_str(), ss.str().size(), 0);

	if (statusCode == 200) {
		std::ofstream file(("www/uploads/" + _requestHeader[HEAD]).c_str());
		file << _requestHeader[BODY];
		file.close();
	}
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
		std::string tmpHeader = rawRequest.substr(0, rawRequest.find("\r\n\r\n"));
		std::string tmpBodyHeader = rawRequest.substr(rawRequest.find("\r\n\r\n") + 4, std::string::npos);
		tmpBodyHeader = tmpBodyHeader.substr(0, tmpBodyHeader.find("\r\n\r\n"));
		std::string tmpBoundary = tmpBodyHeader.substr(0, tmpBodyHeader.find("\r"));
		long contentLength = std::atol(_requestHeader[CONTENT_LENGTH].c_str()) + tmpHeader.length() + 4 + tmpBodyHeader.length() + tmpBoundary.length() + 2;
		if (_method == GET || (_method == POST && contentLength < BUFFER_SIZE)) {
			headerRead = false;
			return (true);
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

Request::~Request(void){
}

int Request::getClientfd(void) const {
	return _clientfd;
}
