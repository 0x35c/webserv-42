#include <iostream>

int main(int ac, char **av)
{
	if (ac != 2)
		return (1);

	std::cout << "Hello world!" << std::endl;
	std::cout << av[1] << std::endl;

	return (0);
}
