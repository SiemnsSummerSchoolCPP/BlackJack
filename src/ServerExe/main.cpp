#include "Networking/BlackJackServer.hpp"
#include <iostream>

int main(const int argc, const char* const* const argv)
{
	if (argc == 1)
	{
		std::cerr << "No port provided" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	const auto port = atoi(argv[1]);
	auto bjServer = Networking::BlackJackServer(port);
	
	while (true)
	{
		bjServer.getServerEntry().waitForAnActivity();
		bjServer.getServerEntry().parseActivity();
	}
	
	return EXIT_SUCCESS;
}
