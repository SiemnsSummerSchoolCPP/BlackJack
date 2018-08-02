#include "BlackJackServer.hpp"
#include "UserModel.h"
#include "SocketTools/Exceptions.h"
#include "MsgHeaders.h"
#include "Utils.h"

#include <iostream>
#include <regex>

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

SocketTools::Server& BlackJackServer::getServerEntry()
{
	return *m_server;
}

/*
** Events
*/

void BlackJackServer::onNewClient(const int socket)
{
	// Server side.
	{
		auto newUser = UserModel();
		newUser.name = getNameOfNewUser(socket);
		m_users[socket] = newUser;
		printf("[New user connected]Socket = %d; name = %s\n",
			socket,
			newUser.name.c_str());
	}
	
	// Newly connected user.
	{
		sendStr(socket,
			STR(MsgHeaders::kOneEndMsg +
				"Successfully connected as " + m_users[socket].name));
	}
	
	// To all other users.
	{
		broadcastStr(
			STR(MsgHeaders::kBroadcastMsg +
				m_users[socket].name + " has connected to the game."),
			socket);
	}
}

void BlackJackServer::onClientDisconnected(int socket)
{
	const auto userName = m_users[socket].name;
	
	// Server.
	{
		printf("[User disconnected] Socket = %d name = %s\n",
			socket,
			userName.c_str());
		m_users.erase(socket);
	}
	
	// Broadcast to everyone.
	{
		broadcastStr(
			STR(MsgHeaders::kBroadcastMsg + "User "
				+ userName + " has disconnected"),
			socket);
	}
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
		sendStr(socket, STR(MsgHeaders::kOneEndMsg + "Invaldi msg header!"));
	}
}

void BlackJackServer::userRequestsBroadcastMsg(
	const int socket,
	const std::string msg)
{
	const auto finalMsg = STR(
		MsgHeaders::kBroadcastMsg +
		"[" + m_users[socket].name + "]: " + msg);
	
	broadcastStr(finalMsg);
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
		
		sendStr(socket,
			STR(MsgHeaders::kOneEndMsg +
				"[Invalid Name]: Name = '" + newName + "'"));
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
		sendStr(socket, STR(
			MsgHeaders::kOneEndMsg +
			"Successfully changed your name from "
			+ oldName + " to " + newName));
	}
	
	// Everybody else
	{
		broadcastStr(
			STR(MsgHeaders::kBroadcastMsg +
				"User " + oldName + " has just changed his name to " + newName),
			socket);
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

/*
** Private helpers.
*/

std::string BlackJackServer::getNameOfNewUser(const int socket)
{
	return STR("Guest" + std::to_string(socket));
}
