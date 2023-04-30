#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <string>
#include <map>
#include "include.hpp"

class Server {
	public:
		Server(std::string ipAddr, int port);
		~Server(void);
		
	// Private member functions
	private:
		Server(void);
		Server(const Server& other);
		Server&	operator= (const Server& other);
		void createServerSocket(void);
		void listenForRequest(void);
		void bindPort(void);
		void acceptRequest(void);
		void readRequest(void);
		void respondToRequest(void);
		void exitWithError(const std::string& errorMessage);
		bool setStatusCode(void);

	// Private attributes
	private:
		int _sockfd;
		int	_clientfd;
		int _port;
		std::string _statusCode;
		std::map<std::string, std::string> _requestHeader;
		std::string _ipAddr;
		const char* _buffer;
		sockaddr_in _sockAddr;
		socklen_t _sockAddr_len;
		
};

#endif
