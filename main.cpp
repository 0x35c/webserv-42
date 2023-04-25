#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <cerrno>
/*
struct in_addr{
	unsigned long s_addr;
};

struct sockaddr_in {
	short sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};
*/

#define PORT 8080

//IP 0.0.0.0 is localhost
#define IP "0.0.0.0"

//start program and connect to "localhost:8080"
int	main()
{
	//create the socket server
	int socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_server < 0)
		return (1);

	sockaddr_in socketAddress;
	socklen_t	socketAddress_len = sizeof(socketAddress);
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(PORT);
	socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind to the PORT with the informations above
	if (bind(socket_server, (sockaddr*)&socketAddress, socketAddress_len) < 0)
	{
		std::cout << strerror(errno) << "\n";
		return (2);
	}

	//start to listen to request (= wait)
	if (listen(socket_server, 1) < 0)
		return (3);
	std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
        << inet_ntoa(socketAddress.sin_addr)
        << " PORT: " << ntohs(socketAddress.sin_port)
        << " ***\n\n";
	std::cout << ss.str() << std::endl;

	//when someone connect, it accept it
	const int BUFFER_SIZE = 30720;
	while (true)
	{
		int client_socket = accept(socket_server, (sockaddr*)&socketAddress, &socketAddress_len);
		if (client_socket < 0)
		{
			std::ostringstream ss;
			ss <<
			"Server failed to accept incoming connection from ADDRESS: "
			<< inet_ntoa(socketAddress.sin_addr) << "; PORT: "
			<< ntohs(socketAddress.sin_port);
			return (4);
		}
		std::cout << "someone connected.\n";
		char buffer[BUFFER_SIZE] = {0};
		if (read(client_socket, buffer, BUFFER_SIZE) < 0)
			return (5);
		std::cout << buffer << "\n";
		std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
           << htmlFile;
		write(client_socket, ss.str().c_str(), ss.str().size());

		close(client_socket);
	}
	close(socket_server);
}
