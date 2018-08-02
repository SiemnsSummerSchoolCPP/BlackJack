#pragma once

#include "SocketIO.hpp"
#include <netinet/in.h>
#include <string>
#include <vector>
#include <functional>

namespace SocketTools
{
	class Server : public SocketIO
	{
	public:
		// Event handlers' types.
		typedef std::function<void (Server&, int)> ClientConnectedEvent;
		typedef std::function<void (Server&, int)> ClientDisconnectedEvent;
		typedef std::function<void (Server&, int, unsigned char*, size_t)>
			ClientMsgEvent;
	
		// Constructors.
		Server(size_t maxNbOfClients = 4, size_t maxPendingConnections = 5);
		
		// Getters.
		const int* getClients() const;
		size_t getMaxNbOfClients() const;
		std::vector<ClientConnectedEvent>& getNewClientEventHandlers();
		std::vector<ClientDisconnectedEvent>& getDisconnectedEventHandlers();
		std::vector<ClientMsgEvent>& getClientMsgEventHandlers();
		
		// Public methods.
		void setup(int port);
		int waitForAnActivity(time_t seconds = -1, suseconds_t micros = 0);
		void parseActivity();
		
		// Status get.
		size_t getNbOfConnections() const;
		
	private:
		const size_t m_maxNbOfClients;
		const size_t m_maxPendingConnections;
	
		int m_masterSocket = 0;
		int* m_clientSockets = nullptr;
		fd_set m_socketDescriptorsSet;
		sockaddr_in m_address;
		
		std::vector<ClientConnectedEvent> m_newClientEventHandlers;
		std::vector<ClientDisconnectedEvent> m_clientDisconnectedEventHandlers;
		std::vector<ClientMsgEvent> m_newMsgEventHandlers;
		
		// Private helpers.
		void addClientSocket(const int newSocket);
		void parseServerActivity();
		void parseClientActivity(size_t clientSocketIndex);
	};
}
