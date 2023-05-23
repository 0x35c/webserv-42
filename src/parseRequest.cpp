#include "Request.hpp"
#include <vector>
#include <algorithm>

void trimString(std::string& string, const char* charset);
static std::string getToken(const std::string& str, char sep, int pos){
	std::string token;
	int cur_pos;

	cur_pos = 1;
	for (uint64_t i = 0; i < str.size(); i++) {
		if (str[i] == sep)	{
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

static std::string getExtension(const std::string& fileName) {
	std::string extension;
	size_t dot = fileName.find(".");
	if (dot != std::string::npos) {
		extension = fileName.substr(dot + 1, fileName.find(",", dot));
	}
	else {
		extension = "text/html";
	}
	return (extension);
}

void Request::processLine(std::string line, int lineToken) {
	std::string str = getToken(line, ' ', 2);
	int pos = str.find('\r');
	std::string root = "www"; // parsing
	if (pos > 0)
		str.erase(pos, 1);
	switch (lineToken) {
		case HEAD:
			{
				str.erase(0, 1);
#if DEBUG
				std::cout << "File name: " << str << "\n";
#endif
				if (line.find("HTTP/1.1") != std::string::npos)
					_validRequest = true;
				else
					_validRequest = false;
				if (_method == "GET" && str.find(root) == std::string::npos) {
					if (str.length() == 0)
						_requestHeader.insert(strPair(HEAD, root + "/index.html")); // parsing
					else
						_requestHeader.insert(strPair(HEAD, root + "/" + str));
				}
				else if (_method == "GET" && str.find(root) != std::string::npos) {
					_requestHeader.insert(strPair(HEAD, str));
				}
				else if (_method == "POST") {
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
			str = getExtension(_requestHeader[HEAD]);
			break;
		default:
			break;
	}
	if (lineToken != HEAD && lineToken != BOUNDARY)
		_requestHeader.insert(strPair(lineToken, str));
}

static t_location& getLocation(const std::string& path, std::vector<t_location>& locations) {
	std::cout << "Header: " << path << std::endl;
	std::cout << "Conf: " << locations[0].locationPath << std::endl;
	for (std::vector<t_location>::iterator it = locations.begin(); it != locations.end(); it++) {
		if (path == it->locationPath)
			return (*it);
	}
	return (*locations.end());
}

int Request::getLineToken(std::string line) {
	if (line.find("POST") != std::string::npos || line.find("GET") != std::string::npos || line.find("DELETE") != std::string::npos) {
		_location = getLocation(_requestHeader[HEAD], _serverConfig.locations);
		return (HEAD);
	}
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
		tmp.erase(tmp.end() - 1);
		size_t pos = tmp.rfind("\n------------------");
		tmp.erase(pos, pos - tmp.length());
		tmp.erase(tmp.end() - 1);
#if DEBUG
		std::cout << tmp;
#endif
		requestHeader[BODY].clear();
		requestHeader[BODY] = tmp;
	}
	else {
		std::cout << str << std::endl;
	}
}

void Request::parseHeader(const std::string& buffer) {
	size_t i = 0;
	int lineToken;
	std::string line;

	while (buffer[i])
	{
		while (buffer[i] && buffer[i] != '\n')
		{
			line += buffer[i];
			i++;
		}
		if (line == "\r")
			break ;
		lineToken = getLineToken(line);
		processLine(line, lineToken);
		line.clear();
		i++;
	}
	line.clear();
	size_t pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		throw std::invalid_argument("invalid header");
	line = buffer.substr(pos + 4, std::string::npos);
	_requestHeader[BODY] = line;
}

bool Request::parseBody(const std::string& buffer) {
	_requestHeader[BODY] += buffer;
	size_t pos = buffer.find(_boundary);
	if (pos != std::string::npos && pos > _boundary.length() + 30) {
#if DEBUG
		/* std::cout << buffer; */
		std::cout << "Body finished" << std::endl;
#endif
		return (true);
	}
	return (false);
}

void Request::respondToRequest(void) {
	if (_method == "GET")
		respondToGetRequest();	
	else if ("POST") {
		processBody(_boundary, _requestHeader[BODY], _requestHeader);
		respondToPostRequest();	
	}
	else if ("DELETE")
		respondToDeleteRequest();	
	_requestHeader.clear();
}
