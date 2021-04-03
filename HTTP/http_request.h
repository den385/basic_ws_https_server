#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"

#include "webserver/HTTP/cookie.h"



namespace net
{
	namespace HTTP
	{
		//--------------------------------------------------------------------------------------------------------------
		// HTTPRequest struct incapsulates data and helper functions for a single transaction from client to server
		// over HTTP(S) protocol.
		//
		// Methods 'getCookie', 'isWSUpgrade', 'isWSHandshake', 'assembleUri' access internal storages of low-level
		// request data (filled in by HTTPParser) to get:
		//
		// 1 either higher-level entitites - HTTP::Cookie / pa::Uri
		// 2 or binary flag reflecting whether current HTTP transaction should open Websocket chanel
		//
		// Struct fields origin and uri are instances of feature rich classes.
		//--------------------------------------------------------------------------------------------------------------
		struct WEBSERVER_API HTTPRequest
		{
			HTTPRequest() = default;
			
			Cookie getCookie(const std::string& name);
			
			bool isWSUpgrade();
			bool isWSHandshake();
			
			void assembleUri(const std::string& uri_path_fragment);
			
		public:
			std::string method;
			int http_version_major = 0;
			int http_version_minor = 0;
			bool is_http = true;									// else - https
			
			std::unordered_map<std::string, std::string> headers;
			std::unordered_map<std::string, Cookie> cookies;
		
			std::string content;
			IPAddress origin;
			Uri uri;
		};
	}
}