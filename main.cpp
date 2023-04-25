#include "include.hpp"

int	set_up_server(sockaddr_in & socketAddress, socklen_t & socketAddress_len)
{
	//create the socket server
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cerr << "socket failed\n";
		return (-1);
	}

	int option = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
	{
		std::cerr << "setsockopt" << "\n";
		return (-1);
	}

	//bind to the PORT with the informations above
	if (bind(sockfd, (sockaddr*)&socketAddress, socketAddress_len) < 0)
	{
		std::cerr << strerror(errno) << "\n";
		return (-1);
	}

	//start to listen to request (= wait)
	if (listen(sockfd, 1) < 0)
	{
		std::cerr << "listen failed\n";
		return (-1);
	}
	std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
        << inet_ntoa(socketAddress.sin_addr)
        << " PORT: " << ntohs(socketAddress.sin_port)
        << " ***\n\n";
	std::cout << ss.str() << std::endl;
	return (sockfd);
}

void respond_to_request(int client_socket, std::string path, std::string type)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	const char* data = new char[fileSize];

	file.read((char *)data, fileSize);
	file.close();

	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\r\n";
	ss << "Content-type: " + type + "\r\n";
	ss << "Content-Length: " << fileSize << "\r\n\r\n";
	ss.write(data, fileSize);
	write(client_socket, ss.str().c_str(), ss.str().size());
	delete [] data;
}

void read_request(int client_socket)
{
	char buffer[30720] = {0};
	if (read(client_socket, buffer, 30720 - 1) < 0)
		return ;
	std::cout << buffer << "\n";
	if (strncmp(buffer, "GET /ulayus.jpg HTTP/1.1", 24) == 0)
		respond_to_request(client_socket, "ulayus.jpg", "image/jpg");
	else if (strncmp(buffer, "GET /images.png HTTP/1.1", 24) == 0)
		respond_to_request(client_socket, "images.png", "image/png");
	else
		respond_to_request(client_socket, "index.html", "text/html");
}

void accept_request(int socket_server, sockaddr_in & socketAddress, socklen_t & socketAddress_len)
{
	while (true)
	{
		//when someone connect, it accept it
		int client_socket = accept(socket_server, (sockaddr*)&socketAddress, &socketAddress_len);
		if (client_socket < 0)
		{
			std::ostringstream ss;
			ss <<
			"Server failed to accept incoming connection from ADDRESS: "
			<< inet_ntoa(socketAddress.sin_addr) << "; PORT: "
			<< ntohs(socketAddress.sin_port);
			return ;
		}
		std::cout << "someone connected.\n";
		read_request(client_socket);
		close(client_socket);
	}
}

void signal_handler(int signal)
{
	if (signal == SIGINT)
		exit(0);
}
//start program and connect to "localhost:8080"
int	main(void)
{
	signal(SIGINT, signal_handler);
	sockaddr_in socketAddress;
	socklen_t	socketAddress_len = sizeof(socketAddress);
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(PORT);
	socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int socket_server = set_up_server(socketAddress, socketAddress_len);
	if (socket_server > 0)
	{
		accept_request(socket_server, socketAddress, socketAddress_len);
		close(socket_server);
	}
}
