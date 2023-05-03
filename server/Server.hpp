#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "include.hpp"

class Server {
	public:
		Server(std::string ipAddr, int port);
		~Server(void);
		
	// Private member functions
	private:
		Server(void);
		Server&	operator= (const Server& other);
		void createServerSocket(void);
		void listenForRequest(void);
		void bindPort(void);

	// Protected member functions
	protected:
		Server(const Server& other);
		void exitWithError(const std::string& errorMessage);

	// Private attributes
	private:
		int _port;
		std::string _ipAddr;

	// Protected attributes
	protected:
		int _sockfd;
		sockaddr_in _sockAddr;
		socklen_t _sockAddr_len;
		
};

#endif
