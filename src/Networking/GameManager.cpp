#include "GameManager.hpp"

using namespace Networking;

GameManager::GameManager(std::map<int, UserModel>& users)
{
	m_state = GameManager::State::kBet;
	
	for (auto& sockAndUser : users)
		addPlayer(sockAndUser.first, sockAndUser.second);
}

/*
** Getters.
*/

GameManager::State GameManager::getState() const
{
	return m_state;
}

size_t GameManager::getNbOfPlayers() const
{
	return m_players.size();
}

bool GameManager::userIdIsPlaying(const int id) const
{
	return (m_players.find(id) != m_players.end());
}

std::vector<int> GameManager::getPlayersIds() const
{
	std::vector<int> ids;
	
	ids.reserve(m_players.size());
	for (const auto& pair : m_players)
    	ids.push_back(pair.first);
	
	return ids;
}

std::map<int, Player*> GameManager::getPlayers()
{
	return m_players;
}

/*
** Setters
*/

void GameManager::setState(const GameManager::State newState)
{
	m_state = newState;
}

/*
** Game actions.
*/

int GameManager::executeBet(
	const int playerId,
	const double amount,
	const int handIndex)
{
	if (!userIdIsPlaying(playerId))
		throw NotAPlayerExecption();
	
	if (amount <= 0)
		throw InvalidBetExecption();
	
	auto& player = *m_players[playerId];
	if (amount > player.getMoney() || player.getMoney() <= 0)
		throw NotEnoughMoneyExeption();
	
	if (handIndex >= player.getHands().size())
		throw InvalidHandIndexExecption();
	
	auto& hand = player.getHands()[handIndex];
	if (hand.bet.amount > std::numeric_limits<double>::epsilon())
		throw AlreadyPlacedABetException();
	
	player.setMoney(player.getMoney() - amount);
	hand.bet.amount += amount;
	
	return 0;
}

/*
** Public methods
*/

void GameManager::addPlayer(const int id, UserModel& userModel)
{
	m_players[id] = createPlayer(userModel);
}

int GameManager::removePlayer(const int id)
{
	auto target = m_players.find(id);
	if (target == m_players.end())
	{
		return -1;
	}
	
	m_players.erase(id);
	delete target->second;
	return 0;
}

/*
** Private helpers.
*/

Player* GameManager::createPlayer(UserModel& userModel) const
{
	return new Player(userModel.name, userModel.money);
}
