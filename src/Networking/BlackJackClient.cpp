#include "BlackJackClient.hpp"
#include "MsgHeaders.h"
#include "Utils.h"
#include "SocketTools/Exceptions.h"
#include <iostream>
#include <regex>

using namespace Networking;

BlackJackClient::BlackJackClient(const char* const ip, const int port)
{
	m_client = new SocketTools::Client();
	
	try
	{
		m_client->setup(ip, port);
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		std::cerr << "Setup: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	m_serverListener = std::thread([&]()
	{
		this->listenToServerActionsThread();
	});
}

void BlackJackClient::listenToServerActionsThread()
{
	while (true)
	{
		const auto receiveResult = m_client->receiveData(m_client->getSocket());
		if (receiveResult == 0 || receiveResult == -1)
		{
			for (const auto& eventHandler : m_onDisconnectEventHandlers)
				eventHandler();
			std::cout << "Disconnected" << std::endl;
			exit(EXIT_SUCCESS);
		}
		
		parseServerInput(
			m_client->getReceiveBuf(),
			m_client->getReceiveBufLen());
	}
}

void BlackJackClient::parseServerInput(
	const unsigned char* const rawMsg,
	const size_t msgLen)
{
	switch (rawMsg[0])
	{
		case MsgHeaders::kOneEndMsg:
			// Fallthrough
		case MsgHeaders::kBroadcastMsg:
			std::cout << (rawMsg + 1) << std::endl;
			break;
	}
}

bool BlackJackClient::parseUserInput(const std::string userInput)
{
	if (parseUserInputSendMsg(userInput)) {}
	else if (parseUserInputChangeName(userInput)) {}
	else
	{
		fprintf(stderr, "%s: Invalid command\n", userInput.c_str());
		return false;
	}
	
	return true;
}

bool BlackJackClient::parseUserInputSendMsg(std::string userInput)
{
	std::smatch regexMatch;
	
	const auto itMatches = std::regex_search(
		userInput,
		regexMatch,
		std::regex("^(?:send message|send msg): (.+)$"));
	
	if (!itMatches)
		return false;
	
	sendStr(STR(MsgHeaders::kBroadcastMsg + regexMatch[1].str()));
	return true;
}

bool BlackJackClient::parseUserInputChangeName(const std::string userInput)
{
	std::smatch regexMatch;
	
	const auto itMatches = std::regex_search(
		userInput,
		regexMatch,
		std::regex("^(?:change name to|set name to) (.+)$"));
	
	if (!itMatches)
		return false;
	
	sendStr(STR(MsgHeaders::kChangeNameRequest + regexMatch[1].str()));
	return true;
}

void BlackJackClient::sendStr(const std::string str)
{
	try
	{
		m_client->sendData(m_client->getSocket(), str.c_str(), str.length());
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		fprintf(stderr, "Failed to send msg: %s\n", e.what());
		return;
	}
}