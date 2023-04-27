#include "Server.hpp"

static void processLine(std::string line, std::map<std::string, std::string>& _requestHeader, int lineNumber) {
	switch (lineNumber) {
		case 1:
			{
				int index = line.find('/');
				int i;
				for (i = index; line[i] != ' '; i++) {}
				if (i == index + 1)
					_requestHeader.insert(std::pair<std::string, std::string>
										 ("PATH", "server/index.html"));
				else
					_requestHeader.insert(std::pair<std::string, std::string>
										 ("PATH", "server/" + line.substr(index + 1, i)));
			}
			break;
		case 2:
			{
				int index = line.find(' ');
				int i;
				for (i = index; line[i] != '\r'; i++) {}
				_requestHeader.insert(std::pair<std::string, std::string>
									 ("HOST", line.substr(index + 1, i)));
			}
			break;
		case 4:
			{
				int index = line.find(' ');
				int i;
				for (i = index; line[i] != ','; i++) {}
				_requestHeader.insert(std::pair<std::string, std::string>
									 ("TYPE", line.substr(index + 1, i)));
			}
			break;
		default:
			break;	
	}

}

static void parseRequest(std::string request, std::map<std::string, std::string> _requestHeader) {
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
}
