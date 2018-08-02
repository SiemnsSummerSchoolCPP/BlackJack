#include "Networking/BlackJackClient.hpp"
#include <iostream>

static std::string getUserInput()
{
	std::string input;
	
	std::getline(std::cin, input);
	if (errno == EINTR)
	{
		std::cin.clear();
		errno = 0;
		return getUserInput();
	}
	
	return input;
}

int main(const int argc, const char* const* const argv)
{
	if (argc < 3)
	{
		std::cerr << "An IP and a port is required." << std::endl;
		std::cout << "Ex: 127.0.0.1 8000" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	const auto ip = argv[1];
	const auto port = atoi(argv[2]);
	
	auto clientEntry = Networking::BlackJackClient(ip, port);
	while (true)
	{
		clientEntry.parseUserInput(getUserInput());
	}
	return 0;
}
