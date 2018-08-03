#include "PlayerHand.h"

using namespace Networking;

std::ostream& Networking::operator << (std::ostream& o, const PlayerHand& h)
{
	o << "Bet: " << h.bet.amount << "$: " << reinterpret_cast<const Hand&>(h);
	return o;
}
