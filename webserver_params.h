#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"



namespace net
{
	//------------------------------------------------------------------------------------------------------------------
	// Struct to pass low-level network info from boost::asio sockets level to higher-level HTTPRequest class instances
	// and then to webengine's WebContext
	struct WebServerParams
	{
		WebServerParams(const std::string& host, uint16_t http_port, uint16_t https_port)
			: local_host(host), local_http_port(http_port), local_https_port(https_port)
		{}
		
		std::string local_host;      // string storage because it can be either DNS-resolved symbolic name or ipV4 address
		uint16_t local_http_port;
		uint16_t local_https_port;
		
		IPAddress remote_ip;         // ipV4 address
		uint16_t remote_port;        // filled but unused currently
		
		std::shared_ptr<SSLContext> context;
	};
}