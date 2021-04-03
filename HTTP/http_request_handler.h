#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"



namespace net
{
	//------------------------------------------------------------------------------------------------------------------
	// HTTPRequestHandler - base class for HTTPBridge. Webserver-Webengine adapter class.
	// CreatorType - a type of a functor, that is used across webserver module to pass HTTP/WS requests to webengine
	// module.
	// Function, satisfying CreatorType, is declared in webengine's HTTPBridge.
	// It's once passed to WebServer, which becomes a factory of HTTPBridge instances, creating one bridge per one
	// HTTPConnection.
	//------------------------------------------------------------------------------------------------------------------
	namespace HTTP
	{
		struct HTTPResponse;
		struct HTTPRequest;
		
		class HTTPRequestHandler
		{
		public:
			using CreatorType = std::function<std::unique_ptr<HTTPRequestHandler>()>;
			
			virtual void HandleRequest(HTTPRequest& req, HTTPResponse& rep) = 0;
		};
	}
}