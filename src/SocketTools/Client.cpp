#include "Client.hpp"
#include "SocketIO.hpp"
#include "Exceptions.h"
#include <sys/socket.h>
#include <cerrno>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define STR(someStr) (std::string() + someStr)

using namespace SocketTools;
using namespace Exceptions;

int Client::getSocket() const
{
	return m_socket;
}

void Client::setup(const char* ip, int port)
{
	if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw SocketExceptionStr(STR("socket: " + std::strerror(errno)));
	
	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	
	if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) == -1)
		throw SocketExceptionStr(STR("inet_pton: " + std::strerror(errno)));
	
	const auto connectResult = connect(
		m_socket,
		reinterpret_cast<sockaddr*>(&serverAddr),
		sizeof(serverAddr));
	
	if (connectResult == -1)
		throw SocketExceptionStr(STR("connect: " + std::strerror(errno)));
}
