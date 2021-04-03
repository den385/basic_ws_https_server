#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"

#include "webserver/connection.h"
#include "webserver/HTTP/http_request_handler.h"
#include "webserver/HTTP/http_parser.h"
#include "webserver/HTTP/http_request.h"
#include "webserver/HTTP/http_response.h"
#include "webserver/WS/ws_connection.h"



namespace net
{
	class WebServer;

	
	
	namespace HTTP
	{
		//--------------------------------------------------------------------------------------------------------------
		// HTTPConnection is a class that encapsulates single thread with boost::asio socket's read-write handlers.
		// HTTPConnections run in parallel above different sockets but under single io_service queue which operates over
		// thread pool (see rpclib module).
		//
		// detailed (maybe outdated) description:
		// -> https://phabricator.megaputer.ru/w/pa7/arch/webserver/overview/
		//
		// Method 'do_read' gets raw bytes from boost::asio socket's buffer and parses them one-by-one to get
		// HTTPRequest class instance, an RFC-complying wrapper for HTTP attributes (headers, method, HTTP version etc.)
		// In case HTTPRequest is websocket handshake by RFC, WSConnection is created. Otherwise, after successful
		// parsing, HTTPRequest is passed to corresponding handler (with application level code) in webengine module.
		//
		// Method 'do_write' uses "scatter-gather I/O" approach: divides HTTPResponse class instance info into separate
		// buffers and writes them into socket.
		//
		// Socket is never simultaneously read and written, it's usage is "half-duplex": 'write' happens only after 'read'
		// completes and vice versa. Reads and writes are additionally separated via 'strand' boost::asio primitive,
		// which is legacy element and can be removed with caution.
		//
		// Methods 'process_ws_handshake', '_generate_ws_handshake_headers', '_create_ws_connection' serve the procedure
		// of WSConnection creation and start-up.
		//
		// Method 'stop' initiates graceful shutdown of all socket operations and closure of boost::asio socket itself. It
		// is only called when boost::asio read or write operation finishes with error, indicating any network error.
		//
		// HTTPConnection prolongs it's own life by passing shared_from_this to boost::asio read/write handlers. When
		// the last shared_ptr is freed (shared_ptr's counter = 0), HTTPConnection is destroyed.
		//
		// HTTPConnection instance is reusable: HTTPResponse instance gets header "Connection: keep-alive" so that
		// client-side sockets won't close and open new HTTPConnection's instances.
		//--------------------------------------------------------------------------------------------------------------
		template<typename TSocket>
		class WEBSERVER_API HTTPConnection : public Connection<TSocket>, public std::enable_shared_from_this<HTTPConnection<TSocket>>
		{
			using Connection<TSocket>::sock_;											// boost::asio socket, unique for this Connection
			using Connection<TSocket>::params;
			using Connection<TSocket>::_cancel;
			using Connection<TSocket>::_shutdown;
			using Connection<TSocket>::_close;
			
		public:
			HTTPConnection(WebServer& webserver, std::shared_ptr<TSocket> sock, WebServerParams params,
				std::unique_ptr<HTTPRequestHandler> bridge);
			
			virtual void Start() override final;
			
		private:
			void do_read();
			void do_write();
			
			void process_ws_handshake(const std::string &content);
			void _generate_ws_handshake_headers();
			void _create_ws_connection(std::shared_ptr<HTTPConnection<TSocket>> self);
			
			void stop();																	// TODO: stop by timeout in case client does not reuse
			
		private:
			WebServer& webserver_;															// backlink for WSConnections management
			std::unique_ptr<HTTPRequestHandler> bridge_;									// adapter class instance for REST requests handling
			
			HTTPRequest request_;															// wrapper of client's request
			HTTPResponse response_;                                                         // wrapper of server's response
			HTTPParser request_parser_;														// 1-char-at-a-time parser
			
			enum { max_buffer_length_ = 8192 };												// TODO: check buffer length handling
			std::array<char, max_buffer_length_> buffer_;									// read-write buffer for socket
			Strand strand_;																	// TODO: legacy - remove, use logical sequencing
			
			static constexpr const bool is_http = std::is_same<TSocket, TCPSocket>::value;	// else is https
		};
	}
}