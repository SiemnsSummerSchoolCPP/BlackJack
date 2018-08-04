#include "Player.hpp"

using namespace Networking;

Player::Player(double& money) :
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

std::ostream& Player::printHands(std::ostream& o) const
{
	for (const auto& hand : m_hands)
	{
		o << "{" << hand << "} ";
	}
	return o;
}
