#pragma once

#include <netinet/in.h>
#include <string>

namespace Networking
{
	class Server
	{
	public:
		struct ServerException;
		struct NotEnoughClientSlots;
		
		Server(size_t maxNbOfClients = 6);
		
		void setup(int port);
		int waitForAnActivity(double microseconds = -1);
		
		int parseServerActivity();
		int parseClientActivity(int clientSocket, std::string& outMsg);
		
		bool socketIsPendingAnActivity(int socket) const;
		
	private:
		static const int maxPendingConnections = 3;
		static const int maxMsgLen = 1024;
		
		const size_t m_maxNbOfClients;
		
		int m_masterSocket = 0;
		int* m_clientSockets = nullptr;
		int m_port = -1;
		fd_set m_socketDescriptorsSet;
		sockaddr_in m_address;
		
		void addClientSocket(int newSocket);
	};
	
	/*
	** Exceptions.
	*/
	
	struct Server::ServerException : public std::exception
	{
	private:
		const std::string m_msg;
		
	public:
		ServerException(const std::string msg = "");
		const char* what() const throw();
	};
	
	struct Server::NotEnoughClientSlots : public ServerException
	{
		const char* what() const throw();
	};
}

