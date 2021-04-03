#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"

#include "webserver/HTTP/http_protocol.h"
#include "webserver/HTTP/cookie.h"



namespace net
{
	namespace HTTP
	{
		//--------------------------------------------------------------------------------------------------------------
		// HTTPResponse struct incapsulates data and helper functions for a single transaction from server to client
		// over HTTP(S) protocol.
		//
		// Method 'setCookie' fills HTTP header out of OOP-style Cookie struct.
		// Methods 'stock_reply' and 'stock_reply_str' are used to fill HTTPResponse's content with piece of HTML which
		// is standard for some HTTP status code. Status codes and HTML pieces are moved to a separate file with consts
		// (http_protocol.h).
		// Method 'to_buffers' and 'status_str_to_buffer' are used to present whole of HTTP response as a series of
		// special boost::asio buffers. This boost::asio::const_buffer instances are specially made for gathered write
		// operation (see "Scatter-Gather I/O" aka "Vectored I/O" approach articles).
		//--------------------------------------------------------------------------------------------------------------
		struct WEBSERVER_API HTTPResponse
		{
			void setCookie(const HTTP::Cookie& c);
			
			std::vector<boost::asio::const_buffer> to_buffers();
			
			static HTTPResponse stock_reply(Schema::StatusCode status);
			
		public:
			Schema::StatusCode status = Schema::StatusCode::undefined;
			
			std::unordered_map<std::string, std::string> headers; // HTTP cookies are stored as headers here
			std::string content;                                  // content is usually set from serialization archives
			
		private:
			boost::asio::const_buffer status_str_to_buffer(Schema::StatusCode status);
			static std::string stock_reply_str(Schema::StatusCode status);
		};
	}
}