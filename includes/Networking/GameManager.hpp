#pragma once

#include "Dealer.hpp"
#include "Player.hpp"
#include "UserModel.h"
#include <map>

namespace Networking
{
	class GameManager
	{
	public:
		enum class State;
		
		// Exceptions.
		struct NotAPlayerExecption : public std::exception {};
		struct InvalidHandIndexExecption : public std::exception {};
		struct AlreadyPlacedABetException : public std::exception {};
		struct InvalidBetExecption : public std::exception {};
		struct NotEnoughMoneyExeption : public std::exception {};
		
		GameManager(std::map<int, UserModel>& users);
		
		// Getters.
		GameManager::State getState() const;
		size_t getNbOfPlayers() const;
		bool userIdIsPlaying(int id) const;
		std::vector<int> getPlayersIds() const;
		
		std::map<int, Player*> getPlayers();
		
		// Setters.
		void setState(GameManager::State newState);
		
		// Game actions.
		int executeBet(int playerId, double amount, int handIndex);
		
		void addPlayer(int id, UserModel& userModel);
		int removePlayer(int id);
		
	private:
		GameManager::State m_state;
		Dealer m_dealer;
		std::map<int, Player*> m_players;
		
		Player* createPlayer(UserModel& userModel) const;
	};
	
	enum class GameManager::State
	{
		kBet,
		kHitStand
	};
}

// Dealer - holds his hand and the deck.

// Players: map<socket, Player>

// Player:
// - name&
// - money&
// - hands: vector<PlayerHand>

// Hand:
// - cards: vector

// PlayerHand: Hand
// - bet: Bet

// Bet:
// - amount: double
