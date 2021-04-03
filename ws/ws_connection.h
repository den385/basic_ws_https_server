#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"

#include "webserver/connection.h"
#include "webserver/WS/ws_protocol.h"
#include "templates/mutex.h"



namespace net
{
	namespace  WS
	{
		//--------------------------------------------------------------------------------------------------------------
		// WSConnection is a class that encapsulates single thread with boost::asio socket's read-write handlers.
		// WSConnections run in parallel above different sockets but under single io_service queue which operates over
		// thread pool (see rpclib module).
		//
		// detailed (maybe outdated) description:
		// -> https://phabricator.megaputer.ru/w/pa7/arch/webserver/overview/
		// -> https://phabricator.megaputer.ru/w/pa7/arch/webserver/ws_impl/
		//
		// This WebSocket implementation is one-way only: after connection is established via WS handshake server pushes
		// messages to client. Any attempt of client to send message through this channel leads to WSConnection stop and
		// destruction.
		//
		// Internally, WSConnection instance consists of a sending queue 'send_q_' guarded by a mutex and a series of
		// write handlers on boost::asio io_service's conveyor.
		//
		// There are 3 cases of enqueuing string (S) and write handler (H):
		//
		// 1) 1S + 1H -- if send_q_ is empty in 'write'
		// 2) 1S + 0H -- if send_q_ is not empty in 'write'; H is added via case 3)
		// 3) 0S + 1H -- if send_q_ is not empty in 'handle_write'
		//
		// Method 'write' serves as both filler of queue and initiator of actual writing ("push"). If 'send_q_' is
		// filled too fast and write handlers don't push at this pace, queue overflow happens and WSConnection stops.
		// Method 'handle_write' serves as both dequeuer and a cycle of actual writing ("pushes") which ends when
		// 'send_q_' is emptied.
		//
		// Method 'stop' closes boost::asio socket gracefully.
		//
		// Through callbacks - ws_open_handler, ws_close_handler, ws_messagein_handler, ws_messageout_handler,
		// ws_error_handler WSConnection is able to give feedback to WebServer.
		// Push of WS messages is initiated from WebEngine module's SessionManager, which forwards call to WebServer.
		// Webengine knows onle pauuid conn_id-s of ws connections.
		//
		// WSConnection prolongs it's own life by passing shared_from_this to boost::asio read/write handlers. When
		// the last shared_ptr is freed (shared_ptr's counter = 0), WSConnection is destroyed & socket ops are stopped.
		//--------------------------------------------------------------------------------------------------------------
		template<typename TSocket>
		class WEBSERVER_API WSConnection : public Connection<TSocket>, public std::enable_shared_from_this<WSConnection<TSocket>>
		{
			using Connection<TSocket>::sock_;
			using Connection<TSocket>::params;
			
			using Connection<TSocket>::_cancel;
			using Connection<TSocket>::_shutdown;
			using Connection<TSocket>::_close;
			
			using TSelf = std::shared_ptr<WSConnection<TSocket>>; // used for async-op sequence life prolongation
			
			// const from websockets' RFC, signifying that frame consists of text (in opposition to control frames) and
			// that this frame contains full text message (in opposition to several text frames chained to form single
			// message)
			static const uchar opcode_one = Schema::WSFinRsvOpcode::one_fragment_text;
		
		public:
			WSConnection(std::shared_ptr<TSocket> sock, WebServerParams params, const Uri& uri, const pauuid& sid,
						 const pauuid& id);
			~WSConnection();
			
			virtual void Start() override final;
			
			template<typename TArg>	void Forward(TArg& a);
		
		private:
			void handle_read(TSelf, std::shared_ptr<NetStreambuf> buf, const error_code& ec, size_t bytes);
			
			// enqueue message & write if queue consists of single message
			void write(const std::string& message, uchar opcode = opcode_one);
			
			// dequeue message & write next message if queue not empty
			void handle_write(TSelf, const error_code &ec, size_t);
			
			void stop();
			
		private:
			Uri uri_;
			pauuid id_;
			
			pauuid session_id_;
			
			std::atomic<bool> closed_;
			
			std::list<std::string> send_q_;
			ptl::mutex send_q_mx_;
		};
	} // namespace WS
} // namespace net