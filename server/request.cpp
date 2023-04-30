#include "Server.hpp"

static std::string getToken(const std::string& str, char sep, int pos){
	std::string token;
	int			cur_pos;

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

static void processGetLine(std::string line, strMap& _requestHeader, int lineToken) {
	std::string str = getToken(line, ' ', 2);
	int pos = str.find('\r');
	if (pos > 0)
		str.erase(pos, 1);
	switch (lineToken) {
		case HEAD:
			{
				if (str == "/")
					_requestHeader.insert(strPair(HEAD, "server/index.html"));
				else {
					str.erase(0, 1);
					_requestHeader.insert(strPair(HEAD, "server/" + str));
				}
			}
			break;
		case ACCEPT:
			str = getToken(str, ',', 1);
			break;
		default:
			break;	
	}
	if (lineToken != HEAD)
		_requestHeader.insert(strPair(lineToken, str));
}

static void processPostLine(std::string line, strMap& _requestHeader, int lineToken) {
	std::string str = getToken(line, ' ', 2);
	int pos = str.find('\r');
	if (pos > 0)
		str.erase(pos, 1);
	switch (lineToken) {
		case HEAD:
			{
				if (str == "/")
					_requestHeader.insert(strPair(HEAD, "server/default"));
				else {
					str.erase(0, 1);
					_requestHeader.insert(strPair(HEAD, "server/" + str));
				}
			}
			break;
		case ACCEPT:
			str = getToken(str, ',', 1);
			break;
		default:
			break;	
	}
	if (lineToken != HEAD)
		_requestHeader.insert(strPair(lineToken, str));
}

static void parseRequest(std::string request, strMap& _requestHeader, int method) {
	int i = 0;
	int lineNumber = 0;
	std::string line;

	while (request[i])
	{
		while ((request[i] && request[i] != '\n' && lineNumber != BODY) || (request[i] && lineNumber == BODY))
		{
			line += request[i];
			i++;
		}	
		if (line == "\r")
			lineNumber = BODY;
		if (method == GET)
			processGetLine(line, _requestHeader, lineNumber);
		else if (method == POST && lineNumber != BODY)
			processPostLine(line, _requestHeader, lineNumber);
		if (lineNumber != BODY) {
			lineNumber++;
			i++;
		}
		if (lineNumber != BODY)
			line.clear();
	}
	_requestHeader.insert(strPair(BODY, line));
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
	char buffer[30720] = {0};
	if (read(_clientfd, buffer, 30720 - 1) < 0)
		exitWithError("error: failed to read client socket\n");
	std::cout << buffer << "\n";
	_method = getMethod(buffer);
	parseRequest(buffer, _requestHeader, _method);
	switch (_method) {
		case GET:
			respondToGetRequest();	
			break;
		case POST:
			respondToPostRequest();	
			break;
		default:
			break;
	}
	_requestHeader.clear();
}
