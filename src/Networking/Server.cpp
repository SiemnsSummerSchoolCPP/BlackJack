#include "Server.hpp"

#include <sys/select.h>
#include <unistd.h>
#include <cerrno>

using namespace Networking;

Server::Server(size_t maxNbOfClients) : m_maxNbOfClients(maxNbOfClients)
{
	m_clientSockets = new int[m_maxNbOfClients];
	std::memset(m_clientSockets, 0, sizeof(int) * maxNbOfClients);
}

/*
** Public methods
*/

bool Server::socketIsPendingAnActivity(const int socket) const
{
	return FD_ISSET(socket, &m_socketDescriptorsSet);
}

// setup() - ServerSetup.cpp

static int addSocketsToReadSet(
	const size_t nbOfSockets,
	const int* const sockets,
	fd_set& socketDescriptorSet)
{
	auto maxSocketDescriptor = -1;
	
	for (int i = 0; i < nbOfSockets; i++)
	{
		const auto socketDescriptor = sockets[i];
		if (socketDescriptor > 0)
		{
			FD_SET(socketDescriptor, &socketDescriptorSet);
		}
		
		if (socketDescriptor > maxSocketDescriptor)
		{
			maxSocketDescriptor = socketDescriptor;
		}
	}
	
	return maxSocketDescriptor;
}

int Server::waitForAnActivity(const double microseconds)
{
	FD_ZERO(&m_socketDescriptorsSet);
	
	// Add master socket to set.
	FD_SET(m_masterSocket, &m_socketDescriptorsSet);
	auto clientsMaxSockDescript = addSocketsToReadSet(
		m_maxNbOfClients,
		m_clientSockets,
		m_socketDescriptorsSet);
	
	const auto maxSocketDescriptor = std::max(
		m_masterSocket,
		clientsMaxSockDescript);
	
	timeval timeout;
	timeout.tv_usec = (int)microseconds;
	
	const auto nbOfReadySockets = select(
		maxSocketDescriptor + 1,
		&m_socketDescriptorsSet,
		nullptr,
		nullptr,
		&timeout);
	
	if (nbOfReadySockets == -1)
		throw ServerException(std::strerror(errno));
	
	return nbOfReadySockets;
}

int Server::parseServerActivity()
{
	socklen_t addrLen = sizeof(m_address);
	const auto newSocket = accept(
		m_masterSocket,
		reinterpret_cast<sockaddr*>(&m_address),
		&addrLen);
	
	if (newSocket == -1)
		throw ServerException(std::strerror(errno));
	
	addClientSocket(newSocket);
	return newSocket;
}

/*
** Return 0 on success and -1 if the client diconnected.
*/

static inline int indexOfInt(
	const int needle,
	const int* const tab,
	const size_t tabLen)
{
	for (int i = 0; i < tabLen; i++)
	{
		if (tab[i] == needle)
			return i;
	}
	
	return -1;
}

int Server::parseClientActivity(const int clientSocket, std::string& outMsg)
{
	char buf[Server::maxMsgLen + 1] = { 0 };
	std::string result = "";
	ssize_t bytesRead;
	
	do
	{
		bytesRead = read(clientSocket, buf, sizeof(buf) - 1);
		
		buf[bytesRead] = 0;
		result += buf;
	} while (bytesRead == sizeof(buf) - 1);
	
	// Client disconnected.
	if (bytesRead == 0 && result == "")
	{
		const auto clientSocketIndex = indexOfInt(
			clientSocket,
			m_clientSockets,
			m_maxNbOfClients);

		m_clientSockets[clientSocketIndex] = 0;
		if (close(clientSocketIndex) == -1)
			throw ServerException(strerror(errno));

		return -1;
	}
	
	outMsg = bytesRead;
	return 0;
}

/*
** Private methods.
*/

void Server::addClientSocket(const int newSocket)
{
	for (int i = 0; i < m_maxNbOfClients; i++)
	{
		if (m_clientSockets[i] <= 0)
		{
			m_clientSockets[i] = newSocket;
			break;
		}
	}
	
	throw NotEnoughClientSlots();
}
