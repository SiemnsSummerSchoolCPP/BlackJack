#pragma once

#include "SocketTools/Server.hpp"
#include "UserModel.h"
#include <map>

namespace Networking
{
	class BlackJackServer
	{
	public:
		BlackJackServer(int port, size_t maxNbOfPlayers = 6);
		
		SocketTools::Server& getServerEntry();
		
	private:
		SocketTools::Server* m_server;
		std::map<int, UserModel> m_users;

		void onNewClient(int socket);
		void onClientDisconnected(int socket);
		void onNewMsg(int socket, const unsigned char* bytes, size_t msgLen);
		
		// Str sender helpers.
		void sendStr(int socket, std::string str);
		void broadcastStr(std::string str, int notToThisSocket = -1);
		
		// User requests.
		void invalidUserRequest(int socket, unsigned char msgHeader);
		void userRequestsBroadcastMsg(int socket, std::string msg);
		void userRequestsChangeName(int socket, std::string newName);
		
		// Helpers.
		std::string getNameOfNewUser(int socket);
	};
}
