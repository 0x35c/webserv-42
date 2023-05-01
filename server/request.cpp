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

static void processLine(std::string line, strMap& _requestHeader, int lineToken, int method) {
	std::string str = getToken(line, ' ', 2);
	int pos = str.find('\r');
	if (pos > 0)
		str.erase(pos, 1);
	switch (lineToken) {
		case HEAD:
			{
				if (str == "/" && method == GET)
					_requestHeader.insert(strPair(HEAD, "server/index.html"));
				else if (method == GET) {
					str.erase(0, 1);
					/* std::cout << str << std::endl; */
					_requestHeader.insert(strPair(HEAD, "server/" + str));
				}
				else if (str == "/" && method == POST)
					_requestHeader.insert(strPair(HEAD, "server/default"));
				else if (method == POST) {
					str.erase(0, 1);
					/* std::cout << str << std::endl; */
					_requestHeader.insert(strPair(HEAD, str));
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

static int getLineToken(std::string line) {
	if (line.find("POST") || line.find("GET") || line.find("DELETE"))
		return (HEAD);
	if (line.find("Host:"))
		return (HOST);
	else if (line.find("User-Agent:"))
		return (USER_AGENT);
	else if (line.find("Accept:"))
		return (ACCEPT);
	else if (line.find("Accept-Language:"))
		return (ACCEPT_LANGUAGE);
	else if (line.find("Accept-Encoding:"))
		return (ACCEPT_ENCODING);
	else if (line.find("Content-Type:"))
		return (CONTENT_TYPE);
	else if (line.find("Content-Length:"))
		return (CONTENT_LENGTH);
	else
		return (-2);
}

static void parseRequest(std::string request, strMap& _requestHeader, int method) {
	int i = 0;
	int lineToken;
	std::string line;

	while (request[i])
	{
		while ((request[i] && request[i] != '\n' && lineToken != BODY) || (request[i] && lineToken == BODY))
		{
			line += request[i];
			i++;
		}	
		if (line == "\r")
			lineToken = BODY;
		else if (lineToken != BODY)
			lineToken = getLineToken(line);
		if (lineToken != BODY)
			processLine(line, _requestHeader, lineToken, method);
		if (lineToken != BODY) {
			line.clear();
			i++;
		}
	}
	if (method == POST) {
		line.erase(0, 2);
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
