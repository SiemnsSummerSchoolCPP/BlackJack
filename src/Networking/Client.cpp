#include <sys/socket.h>
#include <cerrno>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Client.hpp"

using namespace Networking;

Client::Client()
{
}

static inline int convertAddressToBinary(
	const char* const addressStr,
	void* const dest)
{
	return inet_pton(AF_INET, addressStr, dest);
}

void Client::setup(const char* const ip, const int port)
{
	if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw ClientException(std::strerror(errno));
	
	sockaddr_in serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	
	if (convertAddressToBinary(ip, &serverAddr.sin_addr) == -1)
		throw ClientException(std::strerror(errno));
	
	const auto connectResult = connect(
		m_socket,
		reinterpret_cast<sockaddr*>(&serverAddr),
		sizeof(serverAddr));
	
	if (connectResult == -1)
		throw ClientException(std::strerror(errno));
}

std::string Client::read() const
{
	char buf[Client::maxMsgLen + 1];
	ssize_t bytesRead;
	std::string result = "";
	
	do
	{
		bytesRead = ::read(m_socket, buf, Client::maxMsgLen);
		buf[bytesRead] = 0;
		result += buf;
	} while (bytesRead > 0);
	
	if (bytesRead == 0 && result == "")
		throw DisconnectedException();
	
	return result;
}

void Client::send(std::string msg) const
{
	const auto flags = 0;
	if (::send(m_socket, msg.c_str(), msg.length(), flags) == -1)
		throw DisconnectedException();
}
