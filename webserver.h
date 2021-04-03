#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"

#include "webserver/webserver_params.h"
#include "webserver/HTTP/http_connection.h"
#include "webserver/HTTP/http_request_handler.h"
#include "webserver/WS/ws_connection.h"
#include "webserver/WS/ws_protocol.h"



namespace net
{
	//------------------------------------------------------------------------------------------------------------------
	// WebServer is a one-instance web service which
	// 1) handles bidirectional HTTP(S) connections in half-duplex mode (write and read can't happen simultaneously
	// 2) stores push-only WS connections.
	//
	// WebServer is multithreaded equally as ThreadPool of it's io_server.
	//
	// HTTP & HTTPs acceptors are listening for TCP/IP connections on ports
	// specified in WebServerParams struct. Upon asynchronous accept operation
	// HTTPConnection<TCPSocket> object is created.
	//
	// Methods 'Start' and 'Stop' methods manage whether acceptors are open. They are called from webengine module.
	// Method 'WSPush' passes websocket message to client.
	// Method 'WSClose' stops single WS channel.
	// Method 'setup_ssl' creates cyphered context to get a secure socket.
	// Method 'ws_forward' passes a single argument to a particular connection, if it exists. This argument is either a
	//   message for client or unsigned int closure code (standard code is used by default).
	// Methods 'do_accept_...' create HTTPConnection objects above socket.
	// Method 'WSAddSession' allows to manage all WSConnections from WebServer.
	//
	// There are two underlying io_services:
	// 1) main_service - a queue of requests deployed over thread pool.
	//    It is passed to WebServer in already running state. All HTTP(S) & WS read-writes
	//    go through this queue.
	// 2) acceptor_service - separate queue of TCP/IP connections waiting to be processed (service not stored directly)
	//
	// detailed (maybe outdated) description:
	// -> https://phabricator.megaputer.ru/w/pa7/arch/webserver/overview/
	//------------------------------------------------------------------------------------------------------------------
	class WEBSERVER_API WebServer
	{
		DECLARE_NONCOPYABLE(WebServer);
		
		template<typename TSocket>
		friend void HTTP::HTTPConnection<TSocket>::_create_ws_connection(std::shared_ptr<HTTP::HTTPConnection<TSocket>> self);
	
	public:
		explicit WebServer(IOService& main_service, IOService& acceptor_service, WebServerParams params,
		                   HTTP::HTTPRequestHandler::CreatorType http_bridge_creator);
		
		void Start();
		void Stop();	// does not stop existing HTTP and WS connections
		
		bool WSPush(const pauuid& conn_id, std::string const& s);
		bool WSClose(const pauuid& conn_id, uint status_code = WS::Schema::WSClosureStatus::normal);
	
	private:
		void setup_ssl();
		
		template<typename TMap, typename TArg>
		bool ws_forward(const pauuid& conn_id, TMap& ws_map, TArg& a);
		
		void do_accept_http();
		
		void do_accept_https();
		
		template<typename TSocket>
		void WSAddSession(const pauuid& id, std::weak_ptr<WS::WSConnection<TSocket>> wp);
	
	public:
		// callbacks for WSConnection lifecycle, which are filled-in in webengine module, on "WS bridge"
		static std::function<void(const pauuid&, const pauuid&, const Uri&)> ws_onopen;
		static std::function<void(const pauuid&, const std::string&)> ws_onmessagein;
		static std::function<void(const pauuid&, const std::string&)> ws_onmessageout;
		static std::function<void(const pauuid&, const error_code&)> ws_onerror;
		static std::function<void(const pauuid&, const Uri&)> ws_onclose;
	
	private:
		// storage and synchronization for WS and WSS connections
		std::map<pauuid, std::weak_ptr<WS::WSConnection<TCPSocket>>> ws_conns_;
		std::map<pauuid, std::weak_ptr<WS::WSConnection<SSLSocket>>> ws_secure_conns_;
		ptl::mutex ws_conns_mx_;
		
		TCPAcceptor http_acceptor_;
		TCPAcceptor https_acceptor_;
		std::shared_ptr<SSLContext> context_;
		
		// Factory method pattern impl, which passes an instance of HTTPRequestHandler to each HTTPConnection
		HTTP::HTTPRequestHandler::CreatorType http_bridge_creator_;
		
		WebServerParams params_;
		
		IOService& main_service_;
	};
}