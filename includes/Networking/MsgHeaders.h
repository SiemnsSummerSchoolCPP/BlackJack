#pragma once

namespace MsgHeaders
{
	using MsgHeaderType = char;
	
	const MsgHeaderType kOneEndMsg = 1 + 0;
	const MsgHeaderType kBroadcastMsg = 1 + 1;
	
	const MsgHeaderType kChangeNameRequest = 10 + 0;
	const MsgHeaderType kSetUserAsReadyRequest = 10 + 1;
	
	// Game msg headers.
	const MsgHeaderType kBetRequest = 42 + 0;
	const MsgHeaderType kHitRequest = 42 + 1;
	const MsgHeaderType kStandRequest = 42 + 2;
}
