#include "Request.hpp"
#include "Server.hpp"
#include <ctime>
#include <sys/wait.h>

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

bool Request::requestCGI()
{
	std::string fileName = _requestHeader[HEAD];
	for (size_t i = 0; i < _location->executableCGI.size(); i++)
	{
		if (fileName.length() > _location->extensionCGI[i].length() && fileName.substr(fileName.length() - _location->extensionCGI[i].length()) == _location->extensionCGI[i]) {
			_cgi.inCGI = true;
			if (setStatusCode() == 400)
			{
				sendErrorResponse();
				return (false);
			}
			executeCGI(fileName, (char *)_location->executableCGI[i].c_str());
			return (true);
		}
	}
	return (false);
}

void Server::checkCGI(void)
{
	for (requestMap::iterator it = _requests.begin(); it != _requests.end(); it++)
	{
		if (it->second.getCGI().inCGI == false)
			continue ;
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
			std::string responseCGI = buffer;
			if (responseCGI.empty())
				responseCGI = "Content-type: text/html\n\n<html><body><h1>execution of CGI failed</h1></body></html>";
			if (responseCGI.find("\n\n") == std::string::npos || responseCGI.find("Content-type: text/html") == std::string::npos)
				responseCGI = "Content-type: text/html\n\n<html><body><h1>bad CGI header</h1></body></html>";
			std::string tmpHeader = responseCGI.substr(0, responseCGI.find("\n\n") + 1);
			std::string tmpBody = responseCGI.substr(responseCGI.find("\n\n") + 2, std::string::npos);
			size_t contentLength = tmpBody.length();
			ss << "HTTP/1.1 " << it->second.getStatusCode() << "\r\n";
			ss << tmpHeader;
			ss << "Content-Length: " << contentLength << "\r\n\r\n";
			ss << tmpBody;
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
}

void Request::executeCGI(std::string fileName, char *executableCGI) {
	initializeEnvpCGI();
	_cgiEnv["SCRIPT_NAME"] = fileName;
	_cgiEnv["URL"] = fileName;
	if (pipe(_cgi.fds[0]) == -1
		|| pipe(_cgi.fds[1]) == -1)
		throw (Request::RequestException());
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

		_cgiEnv["QUERY_STRING"] = _query;
		char *args[4] = {executableCGI, (char *)(fileName.c_str()),
						(char *)(_query.c_str()),NULL};
		char **EnvpVariables = getEnvpInArray(_cgiEnv);
		execve(executableCGI, args, EnvpVariables);
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
