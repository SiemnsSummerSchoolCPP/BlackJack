#include "Deck.hpp"

using namespace PlayingCards;

/*
** Exceptions
*/

const char* Deck::NotEnoughCards::what() const throw()
{
	return "Not enough cards in deck.";
}

/*
** Cosntructors & destructors.
*/

Deck::Deck() : m_cards(freshDeck())
{
}

Deck::~Deck()
{
	if (m_cards)
	{
		for (int i = 0; i < Deck::nbOfCards; i++)
		{
			if (m_cards[i])
				delete m_cards[i];
		}
		delete[] m_cards;
	}
}

/*
** Public methods
*/

void Deck::shuffle()
{
	std::random_device randomDevice;
	std::mt19937 merseneTwister(randomDevice());
	std::uniform_int_distribution<> uniformDistrib(0, Deck::nbOfCards - 1);
	
	for (auto i = 0; i < Deck::nbOfCards; i++)
	{
		auto index1 = uniformDistrib(merseneTwister) % Deck::nbOfCards;
		auto index2 = uniformDistrib(merseneTwister) % Deck::nbOfCards;
		std::swap(m_cards[index1], m_cards[index2]);
	}
	m_cardIndex = 0;
}

Card& Deck::dealCard()
{
	if (m_cardIndex >= Deck::nbOfCards)
	{
		throw NotEnoughCards();
	}
	
	return *m_cards[m_cardIndex++];
}

/*
** Private static methods
*/

static const Card::Suit suits[] =
{
	Card::Suit::kDiamonds,
	Card::Suit::kSpades,
	Card::Suit::kClubs,
	Card::Suit::kHearts,
};

Card** Deck::freshDeck()
{
	auto cards = new Card*[nbOfCards];
	
	const auto minRank = static_cast<int>(
		std::numeric_limits<Card::Rank>::min());
	const auto maxRank = static_cast<int>(
		std::numeric_limits<Card::Rank>::max());

	auto i = 0;
	for (const auto& suit : suits)
	{
		for (auto rank = minRank; rank <= maxRank; rank++)
		{
			cards[i] = new Card(
				static_cast<Card::Suit>(suit),
				static_cast<Card::Rank>(rank)
			);
			i++;
		}
	}
	
	assert(i == Deck::nbOfCards);
	
	return cards;
}
