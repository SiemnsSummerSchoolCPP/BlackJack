#include "Player.hpp"

using namespace Networking;

Player::Player(const std::string& name, double& money) :
	m_name(name),
	m_money(money)
{
	const auto anEmptyHand = PlayerHand();
	m_hands.push_back(anEmptyHand);
}

double Player::getMoney() const
{
	return m_money;
}

const std::vector<PlayerHand>& Player::getHands() const
{
	return m_hands;
}

std::vector<PlayerHand>& Player::getHands()
{
	return m_hands;
}

void Player::setMoney(const double newValue)
{
	m_money = newValue;
}
