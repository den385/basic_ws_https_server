#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"



namespace net
{
	namespace WS
	{
		// This file is designed to store all constants of WS protocol in good order.
		// The idea is to avoid having protocol-related constants elsewhere.
		namespace Schema
		{
			enum WSClosureStatus							/// See http://tools.ietf.org/html/rfc6455
			{
				normal			= 1000,
				protocol_error	= 1002,
				timeout			= 4020						/// custom code
			};
			
			enum WSFinRsvOpcode								/// See http://tools.ietf.org/html/rfc6455#section-5.2
			{
				one_fragment_text	= 129,
				one_fragment_binary	= 130,
				close_connection	= 136
			};
		} // namespace Schema
	} // namespace WS
} // namespace net