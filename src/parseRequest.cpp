#include "Request.hpp"
#include <vector>
#include <algorithm>
#include <string>

void trimString(std::string& string, const char* charset);
static std::string getToken(const std::string& str, char sep, int pos){
	std::string token;
	int cur_pos;

	cur_pos = 1;
	for (uint64_t i = 0; i < str.size(); ++i) {
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
	std::string root = _location->root;
	std::string index = _location->index;
	if (pos > 0)
		str.erase(pos, 1);
	switch (lineToken) {
		case HEAD:
			{
				str.erase(0, 1);
				std::string rawRoot = root;
				rawRoot.erase(rawRoot.length() - 1, 1);
				if (line.find("HTTP/1.1") != std::string::npos)
					_validRequest = true;
				else
					_validRequest = false;
				if (str.find(rawRoot) == std::string::npos) {
					if (str.length() == 0)
						_requestHeader[HEAD] = root + index;
					else
						_requestHeader[HEAD] = root + str;
				}
				else
					_requestHeader[HEAD] = str;
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

static t_location* getLocation(const std::string& path, std::vector<t_location>& locations) {
	for (std::vector<t_location>::iterator it = locations.begin(); it != locations.end(); ++it) {
		if (path == it->locationPath)
			return (&(*it));
	}
	return (&(*(locations.begin())));
}

int Request::getLineToken(std::string line) {
	if (line.find("POST") != std::string::npos || line.find("GET") != std::string::npos || line.find("DELETE") != std::string::npos) {
		std::string path = getToken(line, ' ', 2);
		size_t pos = path.rfind("/", path.length() - 1);
		if (pos != std::string::npos) {
			path = path.substr(0, pos + 1);
			if (path[0] == '/')
				path.erase(0, 1);
		}
		else if (closedir(opendir(_requestHeader[HEAD].c_str())) == -1) {
			path = _requestHeader[HEAD] + "/";
			if (path[0] == '/')
				path.erase(0, 1);
		}
		else
			path = "/";
		_location = getLocation(path, _serverConfig.locations);
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
	else if (line.find("Transfer-Encoding:") != std::string::npos)
		return (TRANSFER_ENCODING);
	else
		return (-2);
}

static void processBody(std::string& boundary, std::string& line, strMap& requestHeader, std::string& query) {
	int i = 1;
	std::string str = getToken(line, '\n', i);
	trimString(str, "-\r");
	if (!boundary.empty() && str == boundary) {
		++i;
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
			++i;
		}
		std::string tmp = line.substr(line.find("\r\n\r\n"), line.length());
		tmp.erase(0, 4);
		tmp.erase(tmp.end() - 1);
		size_t pos = tmp.rfind("\n------------------");
		tmp.erase(pos, pos - tmp.length());
		tmp.erase(tmp.end() - 1);
		requestHeader[BODY].clear();
		requestHeader[BODY] = tmp;
	}
	else {
		query = line;
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
			++i;
		}
		if (line == "\r")
		{
			break ;
		}
		lineToken = getLineToken(line);
		processLine(line, lineToken);
		line.clear();
		++i;
	}
	line.clear();
	size_t pos = buffer.find("\r\n\r\n");
	line = buffer.substr(pos + 4, std::string::npos);
	_requestHeader[BODY] = line;
}

bool Request::parseChunkedBody(const std::string& buffer) {
	if (!_chunkBufferFull)
	{
		_chunkBuffer += buffer;
		if (buffer.find("\r\n0\r\n", 0) != std::string::npos)
			_chunkBufferFull = true;
		else
			return (false);
	}

	std::size_t pos = 0;
	while (true) {
		std::size_t limPos = _chunkBuffer.find("\r\n", pos);
		long size = std::strtol(_chunkBuffer.substr(pos, limPos - pos).c_str(), NULL, 16);
		if (size == 0) {
			_chunkBuffer.clear();
			_chunkBufferFull = false;
			return (true);
		}
		_requestHeader[BODY] += _chunkBuffer.substr(limPos + 2, size);
		pos = limPos + 2 + size + 2;
	}
}

bool Request::parseBody(const std::string& buffer) {
	if (_requestHeader[TRANSFER_ENCODING] == "chunked")
		return (parseChunkedBody(buffer));

	_requestHeader[BODY] += buffer;
	size_t pos = buffer.find(_boundary);
	if (pos != std::string::npos && pos > _boundary.length() + 30)
		return (true);
	return (false);
}

static void getQuery(std::string& query, std::string& name) {
	size_t delimiter = name.find("?");
	if (delimiter != std::string::npos) {
		query = name.substr(delimiter + 1);
		name = name.substr(0, delimiter);
	}
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

void Request::respondToRequest(void) {
	editName(_requestHeader[HEAD]);
	getQuery(_query, _requestHeader[HEAD]);
	if (_method == "GET")
		respondToGetRequest();
	else if (_method == "POST") {
		processBody(_boundary, _requestHeader[BODY], _requestHeader, _query);
		respondToPostRequest();
	}
	else if (_method == "DELETE")
		respondToDeleteRequest();
	_requestHeader.clear();
	_boundary.clear();
}
