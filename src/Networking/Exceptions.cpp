#include "Server.hpp"
#include "Client.hpp"

using namespace Networking;

Server::ServerException::ServerException(const std::string msg) : m_msg(msg)
{
}

const char* Server::ServerException::what() const throw()
{
	return m_msg.c_str();
}

const char* Server::NotEnoughClientSlots::what() const throw()
{
	return "Not enough slot for new client sockets.";
}

Client::ClientException::ClientException(const std::string msg) : m_msg(msg)
{
}

const char* Client::ClientException::what() const throw()
{
	return m_msg.c_str();
}

const char* Client::DisconnectedException::what() const throw()
{
	return "Disconnected from server.";
}

