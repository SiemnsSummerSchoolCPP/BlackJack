#pragma once

#include "UserModel.h"
#include "GameManager.hpp"
#include "SocketTools/Server.hpp"
#include <map>

namespace Networking
{
	class BlackJackServer
	{
	public:
		BlackJackServer(int port, size_t maxNbOfPlayers = 6);
		
		bool aGameIsTakingPlace() const;
		SocketTools::Server& getServerEntry();
		
	private:
		constexpr static const double initialAmmountOfMoney = 1000;
	
		SocketTools::Server* m_server;
		std::map<int, UserModel> m_users;
		GameManager* m_gameManager = nullptr;

		void onNewClient(int socket);
		void onClientDisconnected(int socket);
		void onNewMsg(int socket, const unsigned char* bytes, size_t msgLen);
		
		// Str sender helpers.
		void sendStr(int socket, std::string str);
		void sendMsg(int socket, std::string msg);
		void broadcastStr(std::string str, int notToThisSocket = -1);
		void broadcastMsg(std::string msg, int notToThisSocket = -1);
		void broadcastMsg(std::string msg, const std::vector<int> sockets);
		
		// User requests.
		void invalidUserRequest(int socket, unsigned char msgHeader);
		void userRequestsBroadcastMsg(int socket, std::string msg);
		void userRequestsChangeName(int socket, std::string newName);
		void userRequestsSetReady(int socket);
		int userRequestsToMakeABet(int socket, std::string msgStr);
		
		// Game.
		void startGame();
		
		// Helpers.
		std::string getNameOfNewUser(int socket);
	};
}
