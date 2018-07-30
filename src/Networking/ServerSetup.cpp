#include "Server.hpp"

#include <cerrno>

using namespace Networking;

static int createMasterSocket()
{
	const auto result = socket(AF_INET , SOCK_STREAM , 0);
	if (result == -1)
		throw Server::ServerException(std::strerror(errno));
	
	return result;
}

static void allowMultipleConnections(const int socket)
{
	int options = true;
	
	const auto setsockoptResult = setsockopt(
		socket,
		SOL_SOCKET,
		SO_REUSEADDR,
		(char*)&options,
		sizeof(options));
	
	if (setsockoptResult == -1)
		throw Server::ServerException(std::strerror(errno));
}

static void bindSocketToLocalHost(
	const int socket,
	const sockaddr* const address)
{
	if (bind(socket, address, sizeof(address)) == -1)
		throw Server::ServerException(std::strerror(errno));
}

void Server::setup(const int port)
{
	m_port = port;
	
	m_masterSocket = createMasterSocket();
	allowMultipleConnections(m_masterSocket);
	
	// Set type of socket created.
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(port);
	
	bindSocketToLocalHost(
		m_masterSocket,
		reinterpret_cast<sockaddr*>(&m_address));
	
	if (listen(m_masterSocket, Server::maxPendingConnections) == -1)
		throw Server::ServerException(std::strerror(errno));
}
