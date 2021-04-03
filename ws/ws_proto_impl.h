#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"



namespace net
{
	namespace WS
	{
		// encode string message or closure code+reason into WS bit format, using lowlevel bitwise operations
		void fill_text_frame(const std::string& message, uchar opcode, std::string& result);
		void fill_closing_frame(uint status, const std::string& reason, std::string& result);
	} // namespace WS
} // namespace net