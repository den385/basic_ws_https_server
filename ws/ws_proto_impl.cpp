#include "webserver/stdafx.h"

#include "webserver/WS/ws_proto_impl.h"



//     Pseudo-graphics showing format of WS frame bit by bit.
//
//     original -> https://tools.ietf.org/html/rfc6455#section-5.2
//
//     BYTE NUMBERS [0; 31] IN 2 ROWS
//
//      0                   1                   2                   3
//      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//     +-+-+-+-+-------+-+-------------+-------------------------------+
//     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
//     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
//     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
//     | |1|2|3|       |K|             |                               |
//     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
//     |     Extended payload length continued, if payload len == 127  |
//     + - - - - - - - - - - - - - - - +-------------------------------+
//     |                               |Masking-key, if MASK set to 1  |
//     +-------------------------------+-------------------------------+
//     | Masking-key (continued)       |          Payload Data         |
//     +-------------------------------- - - - - - - - - - - - - - - - +
//     :                     Payload Data continued ...                :
//     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
//     |                     Payload Data continued ...                |
//     +---------------------------------------------------------------+



namespace net
{
	namespace WS
	{
		void fill_text_frame(const std::string& message, uchar opcode, std::string & result)
		{
			size_t length = message.length();
			
			result.push_back(opcode);
			
			if (length >= 126)													/// unmasked (first length byte < 128)
			{
				int num_bytes;
				if (length > 0xffff)
				{
					num_bytes = 8;
					result.push_back(127);
				}
				else
				{
					num_bytes = 2;
					result.push_back(126);
				}

				uint64_t long_length = static_cast<uint64_t>(length);			/// 8B length
				for (int c = num_bytes - 1; c >= 0; --c)
				{
					result.push_back((long_length >> (8 * c)) % 256);			/// cast char to 8B, shift, cut to 1B
				}
			}
			else
				result.push_back(static_cast<uchar>(length));					/// 1B length
				
			result.append(message);                                             // TODO: check that message need not be
		}                                                                       // TODO: masked & does not start early in lowlevel frame map
		
		void fill_closing_frame(uint status, const std::string& reason, std::string & result)
		{
			result.push_back(status >> 8);
			result.push_back(status % 256);
			result.append(reason);
		}
	} // namespace WS
} // namespace net