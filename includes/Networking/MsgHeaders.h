#pragma once

namespace MsgHeaders
{
	using MsgHeaderType = char;
	
	const MsgHeaderType kOneEndMsg = 1;
	const MsgHeaderType kBroadcastMsg = 2;
	const MsgHeaderType kChangeNameRequest = 10;
}