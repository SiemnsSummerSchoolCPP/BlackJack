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
		struct PlayerIsBustedExeption : public std::exception {};
		struct HandIsAlreadyStandingExeption : public std::exception {};
		
		constexpr static const double blackjackBetMultiplier = (1 + 3.0 / 2.0);
		
		static int getCardPoints(const PlayingCards::Card& card);
		static int computeHandPoints(const Hand& hand);
		static bool isBusted(int points);
		static bool isBlackjack(const Hand& hand);
		
		GameManager(std::map<int, UserModel>& users);
		
		// Getters.
		GameManager::State getState() const;
		size_t getNbOfPlayers() const;
		bool userIdIsPlaying(int id) const;
		std::vector<int> getPlayersIds() const;
		std::map<int, Player*> getPlayers();
		const Hand& getDealersHand() const;
		
		// Setters.
		void setState(GameManager::State newState);
		
		// Game actions.
		int executeBet(int playerId, double amount, int handIndex);
		void dealCardsToEveryone();
		const PlayingCards::Card& executeHit(int playerId, int handIndex);
		void executeStand(int playerId, int handIndex);
		int dealCardsToDealer();
		double getWinMultiplierFromHand(const PlayerHand& hand) const;
		
		// Game status.
		bool everyonePlacedTheirFirstBet() const;
		bool allTheHandsAreStanding() const;
		bool playerCanHit(const Player& player) const;
		
		void addPlayer(int id, UserModel& userModel);
		int removePlayer(int id);
		
	private:
		static const int blackjackPoints = 21;
		static const int dealerPointsLimit = 17;
		
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
