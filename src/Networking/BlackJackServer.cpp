#include "BlackJackServer.hpp"
#include "UserModel.h"
#include "SocketTools/Exceptions.h"
#include "MsgHeaders.h"
#include "Utils.h"

#include <iostream>
#include <regex>
#include <sstream>
#include <algorithm>

using namespace Networking;

BlackJackServer::BlackJackServer(const int port, const size_t maxNbOfPlayers)
{
	m_server = new SocketTools::Server(maxNbOfPlayers);
	
	try
	{
		m_server->setup(port);
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		std::cerr << "Setup: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	const auto newClientEvent = [&](
		const SocketTools::Server& server,
		const int socket)
	{
		(void)server;
		this->onNewClient(socket);
	};
	
	const auto clientDisconnectedEvent = [&](
		const SocketTools::Server& server,
		const int socket)
	{
		(void)server;
		this->onClientDisconnected(socket);
	};
	
	const auto newMsgEvent = [&](
		const SocketTools::Server& server,
		const int socket,
		const unsigned char* const bytes,
		const size_t msgLen)
	{
		(void)server;
		this->onNewMsg(socket, bytes, msgLen);
	};
	
	m_server->getNewClientEventHandlers().push_back(newClientEvent);
	m_server->getDisconnectedEventHandlers().push_back(clientDisconnectedEvent);
	m_server->getClientMsgEventHandlers().push_back(newMsgEvent);
}

/*
** Getters
*/

bool BlackJackServer::aGameIsTakingPlace() const
{
	return (m_gameManager != nullptr);
}

SocketTools::Server& BlackJackServer::getServerEntry()
{
	return *m_server;
}

/*
** Events
*/

void BlackJackServer::onNewClient(const int socket)
{
	auto newUser = UserModel();
	newUser.name = getNameOfNewUser(socket);
	newUser.money = initialAmmountOfMoney;
	newUser.isReady = false;
	m_users[socket] = newUser;

	// Server side.
	{
		printf("[New user connected] Socket = %d; name = %s\n",
			socket,
			newUser.name.c_str());
	}
	
	// Newly connected user.
	{
		sendMsg(socket,
			STR("Successfully connected as " + m_users[socket].name));
	}
	
	// To all other users.
	{
		broadcastMsg(
			STR(m_users[socket].name + " has connected to the game."),
			socket);
	}
}

void BlackJackServer::onClientDisconnected(const int socket)
{
	const auto userName = m_users[socket].name;
	
	if (aGameIsTakingPlace())
	{
		if (m_gameManager->userIdIsPlaying(socket))
		{
			m_gameManager->removePlayer(socket);
		}
	}
	
	// Server.
	{
		printf("[User disconnected] Socket = %d name = %s\n",
			socket,
			userName.c_str());
	}
	
	// Broadcast to everyone.
	{
		broadcastMsg(STR("User " + userName + " has disconnected"), socket);
	}
	
	m_users.erase(socket);
}

void BlackJackServer::onNewMsg(
	const int socket,
	const unsigned char* const bytes,
	const size_t msgLen)
{
	const auto msgStr = std::string(reinterpret_cast<const char*>(bytes) + 1);
	
	switch (bytes[0])
	{
		case MsgHeaders::kBroadcastMsg:
			userRequestsBroadcastMsg(socket, msgStr);
			break;
		case MsgHeaders::kChangeNameRequest:
			userRequestsChangeName(socket, msgStr);
			break;
		case MsgHeaders::kSetUserAsReadyRequest:
			userRequestsSetReady(socket);
			break;
		case MsgHeaders::kBetRequest:
			userRequestsToMakeABet(socket, msgStr);
			break;
		default:
			invalidUserRequest(socket, bytes[0]);
			break;
	}
}

/*
** User requests.
*/

void BlackJackServer::invalidUserRequest(
	const int socket,
	const unsigned char msgHeader)
{
	// Server.
	{
		printf("[INVALID Request] Socket = %d | msgHeader = %d\n",
			socket,
			msgHeader);
	}
	
	// Sender.
	{
		sendStr(socket, STR(MsgHeaders::kOneEndMsg + "Invalid msg header!"));
	}
}

void BlackJackServer::userRequestsBroadcastMsg(
	const int socket,
	const std::string msg)
{
	broadcastMsg(STR("[" + m_users[socket].name + "]: " + msg));
}

void BlackJackServer::userRequestsChangeName(
	const int socket,
	const std::string newName)
{
	if (!std::regex_match(newName, std::regex("^[a-zA-Z0-9]+$")))
	{
		printf("[Failed change name attempt] Socket = %d, NewName = '%s'\n",
			socket,
			newName.c_str());
		
		sendMsg(socket, STR("[Invalid Name]: Name = '" + newName + "'"));
		return;
	}
	
	const auto oldName = m_users[socket].name;
	m_users[socket].name = newName;
	
	// Server
	{
		printf("[Change name] Socket = %d | OldName = '%s' NewName = '%s'",
			socket,
				oldName.c_str(),
				newName.c_str());
	}
	
	// Requester.
	{
		sendMsg(socket,
			STR("Successfully changed your name from "
			+ oldName + " to " + newName));
	}
	
	// Everybody else
	{
		broadcastMsg(
			STR("User " + oldName + " has just changed his name to " + newName),
			socket);
	}
}

void BlackJackServer::userRequestsSetReady(int socket)
{
	auto& user = m_users[socket];
	if (user.isReady)
	{
		sendMsg(socket,
			STR("[Server]: You're already set to Ready status." +
				" Waiting for others."));
	}
	
	user.isReady = true;
	const auto nbOfReadyPlayers = std::count_if(
		m_users.begin(),
		m_users.end(),
		[](const std::pair<int, const UserModel&> pair)
		{
			return pair.second.isReady;
		});

	const auto totalNbOfPlayers = m_users.size();
	assert(nbOfReadyPlayers <= totalNbOfPlayers);
	
	// Server side.
	{
		printf("[User set to Ready]: Socket = %d Name = %s. Game: %ld/%lu\n",
			socket,
			user.name.c_str(),
			nbOfReadyPlayers,
			totalNbOfPlayers);
	}
	
	// Requester.
	{
		sendMsg(socket, STR("Successfully set to Ready."));
	}
	
	// Everyone.
	{
		const auto msg = STR(
			"Player " + user.name + " is now ready. " +
			"Players: " +
			std::to_string(nbOfReadyPlayers) + "/" +
			std::to_string(totalNbOfPlayers) + ". " +
			"Type 'ready' to start the game.");
		broadcastMsg(msg);
	}
	
	if (nbOfReadyPlayers == totalNbOfPlayers)
	{
		startGame();
	}
}

int BlackJackServer::userRequestsToMakeABet(
	const int socket,
	const std::string request)
{
	std::stringstream requestStream(request);
	
	double amount;
	int handIndex;
	
	requestStream >> amount >> handIndex;

	// Server logging.
	{
		printf("[Bet request] Socket = %d Name = %s Bet = %s Hand = %d\n",
			socket,
			m_users[socket].name.c_str(),
			std::to_string(amount).c_str(),
			handIndex);
	}

	if (!aGameIsTakingPlace())
	{
		sendMsg(socket,
			"No game is currently taking place. "
			"Plase type 'ready' and wait for other people to be ready.");
		return -1;
	}
	
	std::string execption = "";
	try
	{
		m_gameManager->executeBet(socket, amount, handIndex);
	}
	catch (const GameManager::NotAPlayerExecption&)
	{
		execption =
			"Sorry, but you're not participating in this game. "
			"Please wait until the game ends.";
	}
	catch (const GameManager::InvalidHandIndexExecption&)
	{
		execption = "You do not own such a hand.";
	}
	catch (const GameManager::AlreadyPlacedABetException&)
	{
		const auto& hand =
			m_gameManager->getPlayers()[socket]->getHands()[handIndex];
		
		execption =
			"You have already placed a bet of " +
			std::to_string(hand.bet.amount) + "$";
	}
	catch (const GameManager::InvalidBetExecption&)
	{
		execption = std::to_string(amount) + "$: invalid bet.";
	}
	catch (const GameManager::NotEnoughMoneyExeption&)
	{
		const auto currentBalance = std::to_string(m_users[socket].money);
		execption =
			"Sorry, but you don't have enough money.\n"
			"\tCurrent balance: " + currentBalance + "$\n" +
			"\tRequested bet: " + std::to_string(amount) + "$";
	}
	
	if (execption != "")
	{
		sendMsg(socket, execption);
		return -1;
	}
	
	const auto user = m_users[socket];
	const auto& hand =
		m_gameManager->getPlayers()[socket]->getHands()[handIndex];
	
	sendMsg(socket, STR(
		"You have successfully placed your bet in amount of: " +
			std::to_string(amount) + "$\n" +
		"Balance: " + std::to_string(user.money) + "$"));

	auto ssBuf = std::stringstream();
	ssBuf
		<< "User " << user.name << " placed a bet of "
		<< amount << "$ on " << reinterpret_cast<const Hand&>(hand);
	broadcastMsg(ssBuf.str());
	
	// Log on the server.
	std::cout << ssBuf.str() << std::endl;
	return 0;
}

/*
** Game
*/

void BlackJackServer::startGame()
{
	const auto nbOfPlayers = m_users.size();
	
	// Server side logging.
	{
		printf("Starting a game of %lu player(s).\n", nbOfPlayers);
	}
	
	// Notify the users.
	{
		broadcastMsg(STR(
		"Everybody is ready. A game of " +
		std::to_string(nbOfPlayers) + " player(s) will now start."));
	}
	
	for (auto& sockAndUser : m_users)
	{
		sockAndUser.second.isReady = false;
	}

	m_gameManager = new GameManager(m_users);
	
	// Remove bankrupts.
	for (const auto& sockAndUser : m_users)
	{
		if (sockAndUser.second.money <= 0)
		{
			sendMsg(
				sockAndUser.first,
				STR("Soory, you don't have enought money to play. Money: " +
					std::to_string(sockAndUser.second.money) + "$"));
			m_gameManager->removePlayer(sockAndUser.first);
		}
	}
	const auto playerSockets = m_gameManager->getPlayersIds();
	
	broadcastMsg(STR(
		"The game has successfully started with a total of " +
		std::to_string(playerSockets.size()) + " player(s)."));
	
	m_gameManager->setState(GameManager::State::kBet);
	broadcastMsg("Betting phase. Please make your bets.", playerSockets);
	
	for (const auto& socket : playerSockets)
	{
		const auto msg = STR(
			"Current money: " + std::to_string(m_users[socket].money) + ". "
			"Make a bet by typing something like: 'bet 42.24$'");
		sendMsg(socket, msg);
	}
}

/*
** Private sender helpers.
*/


void BlackJackServer::sendStr(const int socket, const std::string str)
{
	try
	{
		m_server->sendData(socket, str.c_str(), str.length());
	}
	catch (const SocketTools::Exceptions::SocketException& e)
	{
		fprintf(stderr, "Failed to send str to socket %d: %s\n",
			socket,
			e.what());
	}
}

void BlackJackServer::sendMsg(const int socket, const std::string msg)
{
	sendStr(socket, MsgHeaders::kOneEndMsg + msg);
}

void BlackJackServer::broadcastStr(
	const std::string str,
	const int notToThisSocket)
{
	for (size_t i = 0; i < m_server->getMaxNbOfClients(); i++)
	{
		const auto socket = m_server->getClients()[i];
		if (socket > 0 && socket != notToThisSocket)
		{
			sendStr(socket, str);
		}
	}
}

void BlackJackServer::broadcastMsg(
	const std::string msg,
	const int notToThisSocket)
{
	broadcastStr(MsgHeaders::kBroadcastMsg + msg, notToThisSocket);
}

void BlackJackServer::broadcastMsg(
	const std::string msg,
	const std::vector<int> sockets)
{
	for (const auto& socket : sockets)
	{
		sendStr(socket, msg);
	}
}

/*
** Private helpers.
*/

std::string BlackJackServer::getNameOfNewUser(const int socket)
{
	return STR("Guest" + std::to_string(socket));
}
