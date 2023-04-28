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

static void processLine(std::string line, std::map<std::string, std::string>& _requestHeader, int lineNumber) {
	std::string str = getToken(line, ' ', 2);
	int pos = str.find('\r');
	if (pos > 0)
		str.erase(pos, 1);
	switch (lineNumber) {
		case 1:
			{
				if (str == "/")
					_requestHeader.insert(std::pair<std::string, std::string>
										 ("PATH", "server/index.html"));
				else {
					str.erase(0, 1);
					_requestHeader.insert(std::pair<std::string, std::string>
										 ("PATH", "server/" + str));
				}
			}
			break;
		case 2:
				_requestHeader.insert(std::pair<std::string, std::string>
									 ("HOST", str));
			break;
		case 4:
			{
				std::string token = getToken(str, ',', 1);
				_requestHeader.insert(std::pair<std::string, std::string>
									 ("TYPE", token));
			}
			break;
		default:
			break;	
	}

}

static void parseRequest(std::string request, std::map<std::string, std::string>& _requestHeader) {
	int i = 0;
	int lineNumber = 0;
	std::string line;

	while (request[i])
	{
		while (request[i] != '\n')
		{
			line += request[i];
			i++;
		}	
		lineNumber++;
		i++;
		processLine(line, _requestHeader, lineNumber);
		line.clear();
	}
}

void Server::readRequest(void) {
	char buffer[30720] = {0};
	if (read(_clientfd, buffer, 30720 - 1) < 0)
		exitWithError("error: failed to read client socket\n");
	std::cout << buffer << "\n";
	parseRequest(buffer, _requestHeader);
	respondToRequest();
	_requestHeader.clear();
}
