#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"

#include "webserver/webserver_params.h"



namespace net
{
	//------------------------------------------------------------------------------------------------------------------
	// Base class for HTTP(S) & WS(S) connections. Stores basic web connection management info - boost::asio socket and
	// params of connection.
	//
	// Methods _cancel, _shutdown and _close are used in combination for boost::asio sockets' correct closure.
	template<typename TSocket>
	class Connection
	{
	public:
		Connection(std::shared_ptr<TSocket> sock, WebServerParams p)
			: params(std::move(p)), sock_(std::move(sock))
		{}
		
		virtual ~Connection() = default;
		
		virtual void Start() = 0;
	
	protected:
		void _cancel() {}
		void _shutdown() {}
		void _close() {}
	
	public:
		WebServerParams params;
	
	protected:
		std::shared_ptr<TSocket> sock_;
	};
}