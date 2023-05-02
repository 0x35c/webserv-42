#include "Server.hpp"

static std::string getToken(const std::string& str, char sep, int pos){
	std::string token;
	int cur_pos;

	cur_pos = 1;
	for (uint64_t i = 0; i < str.size(); i++) {
		if (str[i] == sep)	
		{
			if (cur_pos == pos)
				break;
			token.clear();
			cur_pos++;
		}
		else
			token += str[i];
	}
	return (token);
}

void Server::processLine(std::string line, int lineToken) {
	std::string str = getToken(line, ' ', 2);
	int pos = str.find('\r');
	if (pos > 0)
		str.erase(pos, 1);
	switch (lineToken) {
		case HEAD:
			{
				if (str == "/" && _method == GET)
					_requestHeader.insert(strPair(HEAD, "server/index.html"));
				else if (_method == GET) {
					str.erase(0, 1);
					_requestHeader.insert(strPair(HEAD, "server/" + str));
				}
				else if (str == "/" && _method == POST)
					_requestHeader.insert(strPair(HEAD, "server/default"));
				else if (_method == POST) {
					str.erase(0, 1);
					_requestHeader.insert(strPair(HEAD, str));
				}
			}
			break;
		case BOUNDARY:
			{
				std::string tmp = getToken(line, ' ', 3);
				_boundary = tmp.substr(tmp.find("=") + 1, tmp.length());
				trimString(_boundary, "-\r");
			}
			break;
		case ACCEPT:
			str = getToken(str, ',', 1);
			break;
		default:
			break;
	}
	if (lineToken != HEAD && lineToken != BOUNDARY)
		_requestHeader.insert(strPair(lineToken, str));
}

static int getLineToken(std::string line) {
	if (line.find("POST") != std::string::npos || line.find("GET") != std::string::npos || line.find("DELETE") != std::string::npos)
		return (HEAD);
	else if (line.find("Host:") != std::string::npos)
		return (HOST);
	else if (line.find("User-Agent:") != std::string::npos)
		return (USER_AGENT);
	else if (line.find("Accept:") != std::string::npos)
		return (ACCEPT);
	else if (line.find("Accept-Language:") != std::string::npos)
		return (ACCEPT_LANGUAGE);
	else if (line.find("Accept-Encoding:") != std::string::npos)
		return (ACCEPT_ENCODING);
	else if (line.find("Content-Type:") != std::string::npos) {
		if (line.find("boundary") != std::string::npos)
			return (BOUNDARY);
		return (CONTENT_TYPE);
	}
	else if (line.find("Content-Length:") != std::string::npos)
		return (CONTENT_LENGTH);
	else
		return (-2);
}

static void processBody(std::string& boundary, std::string& line, strMap& requestHeader) {
	int i = 1;
	std::string str = getToken(line, '\n', i);	
	trimString(str, "-\r");
	if (str == boundary) {
		i++;
		while (i < 4) {
			str = getToken(line, '\n', i);
			if (i == 2) {
				std::string tmp = getToken(str, '\"', 4);
				requestHeader[HEAD] = tmp;
			}
			else if (i == 3) {
				std::string tmp = getToken(str, ' ', 2);
				requestHeader[CONTENT_TYPE] = tmp;
			}
			i++;
		}
		std::string tmp = line.substr(line.find("\r\n\r\n"), line.length());
		tmp.erase(0, 4);
		requestHeader[BODY].clear();
		requestHeader[BODY] = tmp;
	}
}

void Server::parseRequest(std::string request) {
	size_t i = 0;
	int lineToken;
	std::string line;

	while (request[i])
	{
		while (request[i] && request[i] != '\n')
		{
			line += request[i];
			i++;
		}	
		if (line == "\r") {
			lineToken = BODY;
			break ;
		}
		lineToken = getLineToken(line);
		processLine(line, lineToken);
		line.clear();
		i++;
	}
	line.clear();
	while (i < request.length()) {
		line += request[i];
		i++;
	}
	if (_method == POST) {
		line.erase(0, 2);
		processBody(_boundary, line, _requestHeader);
		_requestHeader.insert(strPair(BODY, line));
	}
}

static int getMethod(std::string buffer) {
	if (buffer.find("GET") != buffer.npos)	
		return (GET);
	else if (buffer.find("POST") != buffer.npos)	
		return (POST);
	else if (buffer.find("DELETE") != buffer.npos)	
		return (DELETE);
	return (ERROR);
}

void Server::readRequest(void) {
	char* buffer_c = new char[BUFFER_SIZE];
	std::string buffer;
	int	end;
	if ((end = read(_clientfd, buffer_c, (BUFFER_SIZE) - 1)) < 0)
		exitWithError("error: failed to read client socket\n");
	write(1, buffer_c, end);
	buffer.assign(buffer_c, end);
	_method = getMethod(buffer);
	parseRequest(buffer);
	switch (_method) {
		case GET:
			respondToGetRequest();	
			break;
		case POST:
			respondToPostRequest();	
			break;
		case DELETE:
			respondToDeleteRequest();	
			break;
		default:
			break;
	}
	_requestHeader.clear();
	delete []buffer_c;
}
