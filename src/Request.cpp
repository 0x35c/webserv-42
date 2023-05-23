#include "Request.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctime>

static char **getEnvpInArray(std::map<std::string, std::string> _cgiEnv);

Request::Request(void)
	: _clientfd(-1), _method("NTM")
{}

Request::Request(int clientfd, const t_server& serverConfig)
	: _clientfd(clientfd), _method("NTM"), _serverConfig(serverConfig)
{}

Request::Request(const Request& other)
	: _clientfd(other._clientfd), _method(other._method), _statusCode(other._statusCode),
		_boundary(other._boundary), _query(other._query),
		_requestHeader(other._requestHeader), _isDirectory(other._isDirectory),
		_serverConfig(other._serverConfig)
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
		_serverConfig = other._serverConfig;
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

void Request::initializeEnvpCGI(void) {
	_cgiEnv["SERVER_SOFTWARE"] = "BOZOSERVER/2.0";
	_cgiEnv["SERVER_NAME"] = "BOZO"; // parsing
	_cgiEnv["SERVER_PORT"] = "8080"; // parsing
	_cgiEnv["SERVER_PROTOCOL"] = "HTTP/1.1";
	_cgiEnv["REQUEST_METHOD"] = _method;
	_cgiEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
	_cgiEnv["CONTENT_TYPE"] = _requestHeader[CONTENT_TYPE];
	_cgiEnv["CONTENT_LENGTH"] = _requestHeader[CONTENT_LENGTH];
	_cgiEnv["REMOTE_ADDR"] = "0.0.0.0"; // parsing
	_cgiEnv["PATH"] = "www/"; // parsing
	_cgiEnv["HTTP_USER_AGENT"] = _requestHeader[USER_AGENT];
}

static char **getEnvpInArray(std::map<std::string, std::string> _cgiEnv) {
	char **arrayEnvpVariable =  new char *[_cgiEnv.size()];
	int i = 0;
	std::string tmp;
	std::map<std::string, std::string>::iterator it;
	for (it = _cgiEnv.begin(); it != _cgiEnv.end(); it++) {
		tmp = it->first + "=" + it->second;
		arrayEnvpVariable[i] = (char *)tmp.c_str();
		i++;
	}
	return (arrayEnvpVariable);
}

// Function is working but we might need to change it 
// to make the read() syscall go through select()
void Request::respondToGetCGI(std::string fileName) {
	initializeEnvpCGI();
	_cgiEnv["SCRIPT_NAME"] = fileName;
	_cgiEnv["URL"] = fileName;
	/* TO-DO LIST
	 * need to save the pid to kill it if we do a ctrl C during the loading of the CGI
	 * change the envp variables of execve to the good one
	 * check return of functions (pipe, dup2, close, read, send, kill)
	*/
	pipe(_cgi.fds[0]);
	pipe(_cgi.fds[1]);
	int pid = fork();
	if (pid == 0)
	{
		dup2(_cgi.fds[0][0], 0);
		dup2(_cgi.fds[1][1], 1);
		close(_cgi.fds[0][0]);
		close(_cgi.fds[0][1]);
		close(_cgi.fds[1][0]);
		close(_cgi.fds[1][1]);

		std::string querys = _requestHeader[HEAD].substr(_requestHeader[HEAD].find("?") + 1, std::string::npos);
		_cgiEnv["QUERY_STRING"] = querys;
		char *args[4] = {(char *)"/usr/bin/python3", (char *)(fileName.c_str()),
						(char *)(querys.c_str()),NULL};
		execve("/usr/bin/python3", args, getEnvpInArray(_cgiEnv));
		//execve("/usr/bin/python3", args, NULL);
		exit(0);
	}
	else if (pid > 0)
	{
		_cgi.pid = pid;
		_cgi.begin_time = std::time(NULL);
		close(_cgi.fds[0][0]);
		close(_cgi.fds[0][1]);
		close(_cgi.fds[1][1]);
	}
}

void Request::respondToGetRequest(void) {
	editName(_requestHeader[HEAD]);
	getQuery(_query, _requestHeader[HEAD]);
#if DEBUG
	std::cout << "ORIGIN: " << _requestHeader[ORIGIN] << std::endl;
	std::cout << "FILENAME: " << _requestHeader[HEAD] << std::endl;
	std::cout << _query << std::endl;
#endif
	DIR* directory = opendir(_requestHeader[HEAD].c_str());
	_isDirectory = false;
	_cgi.inCGI = false;
	if (directory == NULL) {
		std::string fileName = _requestHeader[HEAD].substr(0, _requestHeader[HEAD].find("?"));
		if (fileName.length() > 3 && fileName.substr(fileName.length() - 3) == ".py") {
			_cgi.inCGI = true;
			respondToGetCGI(fileName);
		}
		else {
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

void Request::respondToPostCGI(std::string fileName) {
	initializeEnvpCGI();
	_cgiEnv["SCRIPT_NAME"] = fileName;
	_cgiEnv["URL"] = fileName;
	/* TO-DO LIST
	 * need to save the pid to kill it if we do a ctrl C during the loading of the CGI
	 * change the envp variables of execve to the good one
	 * check return of functions (pipe, dup2, close, read, send, kill)
	*/
	pipe(_cgi.fds[0]);
	pipe(_cgi.fds[1]);
	int pid = fork();
	if (pid == 0)
	{
		dup2(_cgi.fds[0][0], 0);
		dup2(_cgi.fds[1][1], 1);
		close(_cgi.fds[0][0]);
		close(_cgi.fds[0][1]);
		close(_cgi.fds[1][0]);
		close(_cgi.fds[1][1]);

		std::string querys = _requestHeader[BODY];
		_cgiEnv["QUERY_STRING"] = querys;
		char *args[4] = {(char *)"/usr/bin/python3", (char *)(fileName.c_str()),
						(char *)(querys.c_str()),NULL};
		//execve("/usr/bin/python3", args, getEnvpInArray(_cgiEnv));
		execve("/usr/bin/python3", args, NULL);
		exit(0);
	}
	else if (pid > 0)
	{
		_cgi.pid = pid;
		_cgi.begin_time = std::time(NULL);
		close(_cgi.fds[0][0]);
		close(_cgi.fds[0][1]);
		close(_cgi.fds[1][1]);
	}
}

void Request::respondToPostRequest(void) {
	int statusCode = setStatusCode();

	_cgi.inCGI = false;
	std::string fileName = _requestHeader[HEAD].substr(0, _requestHeader[HEAD].find("?"));
	if (fileName.substr(fileName.length() - 3) == ".py") {
		_cgi.inCGI = true;
		respondToPostCGI("www/c" + _requestHeader[HEAD]); // need to correct this
		return ;
	}
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
		long contentLength = std::atol(_requestHeader[CONTENT_LENGTH].c_str()) + tmpHeader.length() + 4 + tmpBodyHeader.length() + tmpBoundary.length() + 2;
		if (_method == "GET" || (_method == "POST" && contentLength < BUFFER_SIZE)) {
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

Request::~Request(void) {
}

int Request::getClientfd(void) const {
	return (_clientfd);
}

t_cgi& Request::getCGI(void) {
	return (_cgi);
}
