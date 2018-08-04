#include "GameManager.hpp"
#include <map>

using namespace Networking;

int GameManager::getCardPoints(const PlayingCards::Card& card)
{
	static const auto points = std::map<PlayingCards::Card::Rank, int>
	{
		{ PlayingCards::Card::Rank::kTwo,	2 },
		{ PlayingCards::Card::Rank::kThree,	3 },
		{ PlayingCards::Card::Rank::kFour,	4 },
		{ PlayingCards::Card::Rank::kFive,	5 },
		{ PlayingCards::Card::Rank::kSix,	6 },
		{ PlayingCards::Card::Rank::kSeven,	7 },
		{ PlayingCards::Card::Rank::kEight,	8 },
		{ PlayingCards::Card::Rank::kNine,	9 },
		{ PlayingCards::Card::Rank::kTen,	10 },
		
		{ PlayingCards::Card::Rank::kJack,	10 },
		{ PlayingCards::Card::Rank::kQueen,	10 },
		{ PlayingCards::Card::Rank::kKing,	10 },
		
		{ PlayingCards::Card::Rank::kAce,	11 },
	};
	
	return points.at(card.getRank());
}

int GameManager::computeHandPoints(const Hand& hand)
{
	int points = 0;
	
	for (const auto& card : hand.cards)
	{
		points += getCardPoints(card);
	}
	
	if (points > blackjackPoints)
	{
		for (const auto& card : hand.cards)
		{
			if (card.getRank() == PlayingCards::Card::Rank::kAce)
			{
				points -= 10;
				if (points <= blackjackPoints)
					break;
			}
		}
	}
	
	return points;
}

bool GameManager::isBusted(const int points)
{
	return points > blackjackPoints;
}

bool GameManager::isBlackjack(const Hand& hand)
{
	return
		(hand.cards.size() == 2) &&
		(computeHandPoints(hand) == blackjackPoints);
}

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

const Hand& GameManager::getDealersHand() const
{
	return m_dealer.getHand();
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

void GameManager::dealCardsToEveryone()
{
	for (auto& idAndPlayer : m_players)
	{
		auto& hand = idAndPlayer.second->getHands()[0];
		m_dealer.dealCardsToAHand(hand);
		
		if (computeHandPoints(hand) >= blackjackPoints)
			hand.standing = true;
	}
	m_dealer.dealCardsToAHand(m_dealer.getHand());
}

const PlayingCards::Card& GameManager::executeHit(
	const int playerId,
	const int handIndex)
{
	if (!userIdIsPlaying(playerId))
		throw NotAPlayerExecption();
	
	auto& player = *m_players[playerId];
	if (!playerCanHit(player))
		throw PlayerIsBustedExeption();

	if (handIndex >= player.getHands().size())
		throw InvalidHandIndexExecption();
	
	auto& hand = player.getHands()[handIndex];
	if (hand.standing)
		throw HandIsAlreadyStandingExeption();
	
	const auto& newCard = m_dealer.dealACard();
	hand.cards.push_back(newCard);
	if (computeHandPoints(hand) >= blackjackPoints)
	{
		hand.standing = true;
	}
	
	return newCard;
}

void GameManager::executeStand(
	const int playerId,
	const int handIndex)
{
	if (!userIdIsPlaying(playerId))
		throw NotAPlayerExecption();
	
	auto& player = *m_players[playerId];
	if (handIndex >= player.getHands().size())
		throw InvalidHandIndexExecption();
	
	auto& hand = player.getHands()[handIndex];
	if (hand.standing)
		throw HandIsAlreadyStandingExeption();
	
	hand.standing = true;
}

int GameManager::dealCardsToDealer()
{
	int cardsDealt = 0;
	
	while (computeHandPoints(m_dealer.getHand()) < dealerPointsLimit)
	{
		m_dealer.getHand().cards.push_back(m_dealer.dealACard());
		cardsDealt++;
	}
	return cardsDealt;
}

double GameManager::getWinMultiplierFromHand(const PlayerHand& hand) const
{
	const auto playerPoints = computeHandPoints(hand);
	
	if (isBusted(playerPoints))
	{
		return 0;
	}
	
	const auto dealerPoints = computeHandPoints(m_dealer.getHand());
	if (isBusted(dealerPoints) || playerPoints > dealerPoints)
	{
		if (isBlackjack(hand))
		{
			return blackjackBetMultiplier;
		}
		
		return 2;
	}
	else if (playerPoints < dealerPoints)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
** Game status
*/

bool GameManager::everyonePlacedTheirFirstBet() const
{
	for (const auto& idAndPlayer : m_players)
	{
		const auto hand = idAndPlayer.second->getHands()[0];
		if (hand.bet.amount <= std::numeric_limits<double>::epsilon())
		{
			return false;
		}
	}
	return true;
}

bool GameManager::playerCanHit(const Player& player) const
{
	for (const auto& hand : player.getHands())
	{
		if (!hand.standing)
			return true;
	}
	
	return false;
}

bool GameManager::allTheHandsAreStanding() const
{
	for (const auto& sockAndPlayer : m_players)
	{
		for (const auto& hand : sockAndPlayer.second->getHands())
		{
			if (!hand.standing)
				return false;
		}
	}
	return true;
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
	return new Player(userModel.money);
}
