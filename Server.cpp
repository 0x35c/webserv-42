#include "Server.hpp"

Server::Server(){
}

Server::~Server(){
}

Server::Server(const Server &copy){
	*this = copy;
}

Server & Server::operator=(const Server &copy){
	if (&copy != this)
	{

	}
	return (*this);
}
