#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"

#include "webserver/HTTP/http_protocol.h"
#include "templates/secure_alloc.h"



namespace net
{
	namespace HTTP
	{
		struct HTTPRequest;
		
		//--------------------------------------------------------------------------------------------------------------
		// HTTPParser gets byte array as input and returns
		// 1 either result code 'good' plus filled 'HTTPRequest' structure
		// 2 or result code 'bad' and HTTP error status code with corresponding message
		// 3 or result code 'indeterminate' which means that byte array contained just a part of valid HTTP message
		//
		// Method 'Parse' returns result plus, possibly, fills HTTPRequest; 'begin' and 'end' are byte array boundaries.
		// Method 'Reset' is called when parser gets solid result - 'good' or 'bad'.
		//
		// struct 'HTTPParser::Data' is an intermediate form to store parsed info.
		//
		// Method 'parse_impl' implements finite automaton over HTTP standard syntax and fills struct 'HTTPParser::Data'
		// Method 'fill_request' carefully builds HTTPRequest from struct Data in case byte buffer contained full
		// message.
		//
		// Methods is_char, is_ctl, is_tspecial, is_digit and state_ variable are used in actual low-level parsing.
		//
		// Parser uses ptl::SecureAlloc allocator and standalone zeroout function to handle http content securely.
		//--------------------------------------------------------------------------------------------------------------
		class HTTPParser
		{
			struct Data
			{
				std::vector<HTTPHeader> headers = {};
				std::string method = "";
				std::string uri = "";
				
				std::vector<char, ptl::SecureAlloc<char>> content;
				
				int http_version_major = 0;
				int http_version_minor = 0;
				int content_length = 0;
			};
			
		public:
			enum class Result { good, bad, indeterminate };
		
		public:
			HTTPParser();
			
			void Reset();
			
			Result Parse(HTTPRequest& req, char* begin, char* end);
			
		private:
			Result parse_impl(char* begin, char* end);
			void fill_request(HTTPRequest& req);
			
			static bool is_char(int c);
			static bool is_ctl(int c);
			static bool is_tspecial(int c);
			static bool is_digit(int c);
			
		private:
			Data data_;
			Schema::ParserState state_;
		};
	}
}