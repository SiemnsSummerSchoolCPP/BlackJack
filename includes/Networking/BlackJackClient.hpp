#pragma once

#include "SocketTools/Client.hpp"
#include <thread>
#include <functional>
#include <vector>

namespace Networking
{
	class BlackJackClient
	{
	public:
		BlackJackClient(const char* ip, int port);
		
		bool parseUserInput(std::string userInput);
		
	private:
		SocketTools::Client* m_client;
		std::thread m_serverListener;
		
		std::vector<std::function<void ()>> m_onDisconnectEventHandlers;
		
		void listenToServerActionsThread();
		void parseServerInput(const unsigned char* rawMsg, size_t msgLen);
		
		// User input parsering.
		bool parseUserInputSendMsg(std::string userInput);
		bool parseUserInputChangeName(std::string userInput);
		bool parseUserInputSetToReady(std::string userInput);
		
		bool parseUserInputPlaceBet(std::string userInput);
		bool parseUserInputHit(std::string userInput);
		bool parseUserInputStand(std::string userInput);
		
		// Msg Sending.
		void sendStr(std::string str);
	};
}
