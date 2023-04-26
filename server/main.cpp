#include "Server.hpp"

//start program and connect to "localhost:8080"
int	main(void)
{
	Server server("0.0.0.0", 8080);

	return (EXIT_SUCCESS);
}
