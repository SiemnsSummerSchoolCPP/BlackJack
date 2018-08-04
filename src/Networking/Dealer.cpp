#include "Dealer.hpp"

using namespace Networking;

Dealer::Dealer()
{
	m_deck.shuffle();
}

Hand& Dealer::getHand()
{
	return m_hand;
}

const Hand& Dealer::getHand() const
{
	return m_hand;
}

const PlayingCards::Card& Dealer::dealACard()
{
	return m_deck.dealCard();
}

void Dealer::dealCardsToAHand(Hand& hand, const size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		hand.cards.push_back(dealACard());
	}
}
