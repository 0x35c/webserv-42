#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include "Server.hpp"

class Request: protected Server {
	public:
		Request(const Server& other);
		~Request(void);

		// Member functions
		void acceptRequest(void);

	private:
		Request(void);
		Request(const Request& other);
		Request&	operator= (const Request& other);
		void readRequest(void);
		void respondToGetRequest(void);
		void respondToPostRequest(void);
		void respondToDeleteRequest(void);
		void errorOnRequest(void);
		void processLine(std::string line, int lineToken);
		void parseRequest(std::string request);
		bool setStatusCode(void);

	private:
		int	_clientfd;
		int _method;
		std::string _statusCode;
		std::string _boundary;
		const char* _buffer;
		strMap _requestHeader;
		
};

#endif