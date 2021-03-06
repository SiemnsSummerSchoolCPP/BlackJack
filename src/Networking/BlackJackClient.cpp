#include "BlackJackClient.hpp"
#include "MsgHeaders.h"
#include "Utils.h"
#include "SocketTools/Exceptions.h"
#include <iostream>
#include <sstream>
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
		const auto receiveResult =
			m_client->getSocketIOTools().receiveData(m_client->getSocket());
		if (receiveResult == 0 || receiveResult == -1)
		{
			for (const auto& eventHandler : m_onDisconnectEventHandlers)
				eventHandler();
			std::cout << "Disconnected" << std::endl;
			exit(EXIT_SUCCESS);
		}
		
		parseServerInput(
			m_client->getSocketIOTools().getReceiveBuf(),
			m_client->getSocketIOTools().getReceiveBufLen());
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
	else if (parseUserInputSetToReady(userInput)) {}
	else if (parseUserInputPlaceBet(userInput)) {}
	else if (parseUserInputHit(userInput)) {}
	else if (parseUserInputStand(userInput)) {}
	else
	{
		fprintf(stderr, "%s: Invalid command\n", userInput.c_str());
		return false;
	}
	
	return true;
}

bool BlackJackClient::parseUserInputSendMsg(const std::string userInput)
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

bool BlackJackClient::parseUserInputSetToReady(const std::string userInput)
{
	std::smatch regexMatch;
	
	const auto itMatches = std::regex_search(
		userInput,
		regexMatch,
		std::regex("^(I am ready|ready|[Ss]tart the game)$"));
	
	if (!itMatches)
		return false;
	
	sendStr(STR(MsgHeaders::kSetUserAsReadyRequest));
	return true;
}

bool BlackJackClient::parseUserInputPlaceBet(const std::string userInput)
{
	static const char* const regexDouble =
		"(?:0|(?:[1-9][0-9]*))(?:\\.[0-9]+)?";

	std::smatch regexMatch;
	double amount;
	int handIndex = 0;
	
	const auto itMatches = std::regex_search(
		userInput,
		regexMatch,
		std::regex(STR("^bet (" + regexDouble + ")(?:\\$)?$")));
	
	if (!itMatches)
	{
		const auto itMatches = std::regex_search(
			userInput,
			regexMatch,
			std::regex(STR("^bet (" + regexDouble + ")(?:\\$)? on ([0-9])$")));
		
		if (!itMatches)
			return false;
		
		handIndex = std::stoi(regexMatch[2]);
	}
	
	amount = std::stod(regexMatch[1]);
	
	auto ssBuf = std::stringstream();
	ssBuf << MsgHeaders::kBetRequest << amount << " " << handIndex;
	
	sendStr(ssBuf.str());
	return true;
}

bool BlackJackClient::parseUserInputHit(const std::string userInput)
{
	std::smatch regexMatch;
	int handIndex = 0;
	
	const auto itMatches = std::regex_search(
		userInput,
		regexMatch,
		std::regex(STR("^([Hh]it|[Hh])$")));
	
	if (!itMatches)
	{
		const auto itMatches = std::regex_search(
			userInput,
			regexMatch,
			std::regex(STR("^[Hh]it on ([0-9])$")));
		
		if (!itMatches)
			return false;
		
		handIndex = std::stoi(regexMatch[1]);
	}
	
	auto ssBuf = std::stringstream();
	ssBuf << MsgHeaders::kHitRequest << handIndex;
	
	sendStr(ssBuf.str());
	return true;
}

bool BlackJackClient::parseUserInputStand(const std::string userInput)
{
	std::smatch regexMatch;
	int handIndex = 0;
	
	const auto itMatches = std::regex_search(
		userInput,
		regexMatch,
		std::regex(STR("^([Ss]tand|[Ss])$")));
	
	if (!itMatches)
	{
		const auto itMatches = std::regex_search(
			userInput,
			regexMatch,
			std::regex(STR("^[Ss]tand on ([0-9])$")));
		
		if (!itMatches)
			return false;
		
		handIndex = std::stoi(regexMatch[1]);
	}
	
	auto ssBuf = std::stringstream();
	ssBuf << MsgHeaders::kStandRequest << handIndex;
	
	sendStr(ssBuf.str());
	return true;
}

void BlackJackClient::sendStr(const std::string str)
{
	try
	{
		m_client->getSocketIOTools().sendData(
			m_client->getSocket(),
			str.c_str(),
			str.length());
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		fprintf(stderr, "Failed to send msg: %s\n", e.what());
		return;
	}
}
