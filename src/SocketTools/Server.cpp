#include "Server.hpp"
#include "Exceptions.h"
#include <memory>
#include <cerrno>
#include <sys/select.h>
#include <unistd.h>

#define STR(someStr) (std::string() + someStr)

using namespace SocketTools;
using namespace SocketTools::Exceptions;

Server::Server(
	const size_t maxNbOfClients,
	const size_t maxPendingConnections) :
	m_maxNbOfClients(maxNbOfClients),
	m_maxPendingConnections(maxPendingConnections)
{
	m_clientSockets = new int[m_maxNbOfClients];
	std::memset(
		m_clientSockets,
		0,
		sizeof(*m_clientSockets) * m_maxNbOfClients);
}

/*
** Getters
*/

const int* Server::getClients() const
{
	return m_clientSockets;
}

size_t Server::getMaxNbOfClients() const
{
	return m_maxNbOfClients;
}

std::vector<Server::ClientConnectedEvent>& Server::getNewClientEventHandlers()
{
	return m_newClientEventHandlers;
}

std::vector<Server::ClientDisconnectedEvent>&
Server::getDisconnectedEventHandlers()
{
	return m_clientDisconnectedEventHandlers;
}

std::vector<Server::ClientMsgEvent>& Server::getClientMsgEventHandlers()
{
	return m_newMsgEventHandlers;
}

/*
** Setup.
*/

static int createMasterSocket()
{
	const auto result = socket(AF_INET, SOCK_STREAM, 0);
	if (result == -1)
		throw SocketExceptionStr(STR("socket: " + std::strerror(errno)));
	
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
		throw SocketExceptionStr(STR("setsockopt: " + std::strerror(errno)));
}

static void bindSocketToLocalHost(
	const int socket,
	const sockaddr* const address)
{
	if (bind(socket, address, sizeof(*address)) == -1)
		throw SocketExceptionStr(STR("bind: " + std::strerror(errno)));
}

void Server::setup(const int port)
{
	m_masterSocket = createMasterSocket();
	allowMultipleConnections(m_masterSocket);
	
	// Set type of socket created.
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(port);
	
    bindSocketToLocalHost(
		m_masterSocket,
		reinterpret_cast<sockaddr*>(&m_address));
	
	if (listen(m_masterSocket, m_maxPendingConnections) == -1)
		throw SocketExceptionStr(STR("listen: " + std::strerror(errno)));
}

static int addSocketsToReadSet(
	const size_t nbOfSockets,
	const int* const sockets,
	fd_set& socketDescriptorSet)
{
	auto maxSocketDescriptor = -1;
	
	for (size_t i = 0; i < nbOfSockets; i++)
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

int Server::waitForAnActivity(
	const time_t seconds,
	const suseconds_t microseconds)
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
	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = seconds;
	timeout.tv_usec = microseconds;

	const auto nbOfReadySockets = select(
		maxSocketDescriptor + 1,
		&m_socketDescriptorsSet,
		nullptr,
		nullptr,
		(seconds < 0) ? nullptr : &timeout);

	if (nbOfReadySockets == -1)
		throw SocketExceptionStr(STR("select: " + std::strerror(errno)));

	return nbOfReadySockets;
}

void Server::parseActivity()
{
	if (FD_ISSET(m_masterSocket, &m_socketDescriptorsSet))
		parseServerActivity();
	
	for (size_t i = 0; i < m_maxNbOfClients; i++)
	{
		if (FD_ISSET(m_clientSockets[i], &m_socketDescriptorsSet))
			parseClientActivity(i);
	}
}

/*
** State getters.
*/

size_t Server::getNbOfConnections() const
{
	size_t count = 0;
	
	for (size_t i = 0; i < m_maxNbOfClients; i++)
	{
		if (m_clientSockets[i] > 0)
			count += 1;
	}
	return count;
}

/*
** Helpers
*/

void Server::addClientSocket(const int newSocket)
{
	for (size_t i = 0; i < m_maxNbOfClients; i++)
	{
		if (m_clientSockets[i] <= 0)
		{
			m_clientSockets[i] = newSocket;
			return;
		}
	}
	
	throw SocketExceptionStr("Not enough client slots.");
}

void Server::parseServerActivity()
{
	socklen_t addrLen = sizeof(m_address);
	const auto newSocket = accept(
		m_masterSocket,
		reinterpret_cast<sockaddr*>(&m_address),
		&addrLen);

	if (newSocket == -1)
		throw SocketExceptionStr(STR("accept: " + std::strerror(errno)));

	addClientSocket(newSocket);
	for (const auto& eventHandler : m_newClientEventHandlers)
		eventHandler(*this, newSocket);
}

void Server::parseClientActivity(const size_t clientSocketIndex)
{
	const auto clientSock = m_clientSockets[clientSocketIndex];
	const auto receiveResult = receiveData(clientSock);
	if (receiveResult > 0)
	{
		for (const auto& eventHandlerF : m_newMsgEventHandlers)
		{
			eventHandlerF(
				*this,
				 clientSock,
				 getReceiveBuf(),
				 getReceiveBufLen());
		}
	}
	else if (receiveResult == 0)
	{
		m_clientSockets[clientSocketIndex] = 0;
		if (close(clientSock) == -1)
			throw SocketExceptionStr(STR("close: " + std::strerror(errno)));

		for (const auto& eventHandlerF : m_clientDisconnectedEventHandlers)
			eventHandlerF(*this, clientSock);
	}
	else
	{
		throw SocketExceptionStr(STR("read: " + std::strerror(errno)));
	}
}
