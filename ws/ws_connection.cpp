#include "webserver/stdafx.h"

#include "webserver/WS/ws_connection.h"
#include "webserver/webserver.h"
#include "webserver/WS/ws_proto_impl.h"



namespace net
{
	namespace WS
	{
		template<typename TSocket>
		WSConnection<TSocket>::WSConnection(std::shared_ptr<TSocket> sock, WebServerParams params, const Uri& uri, const pauuid& sid,
											const pauuid& id)
			: Connection<TSocket>(sock, params), uri_(uri), closed_(false), session_id_(sid), id_(id)
		{}
		
		template<typename TSocket>
		WSConnection<TSocket>::~WSConnection()
		{
			if (!closed_)
			{
				IFLOG(P5, "INFO: ~WSConnection - connection not closed in DTOR occurred.");
				
				stop();
			}
		}
		
		template<typename TSocket>
		void WSConnection<TSocket>::Start()
		{
			WebServer::ws_onopen(id_, session_id_, uri_);
			
			auto buf = std::make_shared<NetStreambuf>(2);
			
			async_read(*sock_.get(), *buf, transfer_exactly(2),						/// expect frame from client
				boost::bind(&WSConnection<TSocket>::handle_read, this, this->shared_from_this(), buf, _1, _2)
			);
		}
		
		template<typename TSocket>													/// get opcode, check mask, log, stop
		void WSConnection<TSocket>::handle_read(TSelf, std::shared_ptr<NetStreambuf> buf, const error_code& ec, size_t bytes)
		{
			if (ec && ec != basic_errors::operation_aborted)
				WebServer::ws_onerror(id_, ec);
			
			if (bytes != 2)
			{
				IFLOG(P5, "INFO: WS read, follows bytes expected vs. bytes transferred.", 2, bytes);
			}
			else
			{
				std::istream is(buf.get());
				uchar opcode = is.get();
				uchar mask = is.get();
				
				if (mask < 128) {                                                              /// unmasked frame = proto error
					IFLOG(P5, "INFO: WS unmasked frame from client. Protocol error.");
				}
				else if (8 == opcode & 0x0f) {                                                 /// close frame opcode
					IFLOG(P5, "INFO: WS close frame from client - connection closure is expected.")
				}
				else {
					IFLOG(P5, "INFO: WS not-closing masked frame from client.");
				}
			}
			
			IFLOG(P5, "INFO: WS frame from client on oneway proto impl. Stopping connection with following id.", id_);
			
			stop();
		}
		
		
		
		template<typename TSocket>
		void WSConnection<TSocket>::write(const std::string& message, uchar opcode)
		{
			std::string result;
			fill_text_frame(message, opcode, result);
			
			if (WebServer::ws_onmessageout)
				WebServer::ws_onmessageout(id_, result);
			
			{
				boost::lock_guard<ptl::mutex> lck(send_q_mx_);
				
				send_q_.push_back(std::move(result));
				
				size_t qsz = send_q_.size();
				
				if (qsz > 100)
				{
					IFLOG(P2, "WS send_q overflow. Size follows.", qsz);
					stop();
				}
				
				if (send_q_.size() == 1) {
					async_write(*sock_.get(), boost::asio::const_buffers_1(send_q_.front().data(), send_q_.front().size()),
						boost::bind(&WSConnection<TSocket>::handle_write, this, this->shared_from_this(), _1, _2)
					);
				}
			}
		}
		
		template<typename TSocket>
		void WSConnection<TSocket>::handle_write(TSelf, const error_code &ec, size_t)
		{
			if (ec)
			{
				if (ec != basic_errors::operation_aborted)
					WebServer::ws_onerror(id_, ec);
				
				boost::lock_guard<ptl::mutex> lck(send_q_mx_);
				
				send_q_.clear();
				
				stop();
				return;
			}
			
			{
				boost::lock_guard<ptl::mutex> lck(send_q_mx_);
				
				send_q_.erase(send_q_.begin());
				
				if (send_q_.size() > 0)
				{
					IFLOG(P5, "WSConnection - internal queue size follows.", send_q_.size());
				
					async_write(*sock_.get(), boost::asio::const_buffers_1(send_q_.front().data(), send_q_.front().size()),
						boost::bind(&WSConnection<TSocket>::handle_write, this, this->shared_from_this(), _1, _2)
					);
				}
			}
		}
		
		
		
		template<typename TSocket>
		void WSConnection<TSocket>::stop()
		{
			bool expect_F = false, desire_T = true;
			bool exchanged = closed_.compare_exchange_strong(expect_F, desire_T);	/// if {false -> true}, impl close
			if (!exchanged)															/// close only once
				return;

			WebServer::ws_onclose(id_, uri_);
			
			_cancel();		/// Cancel all asynchronous operations associated with the socket. Expect operation_aborted.
			_shutdown();	/// Disable sends or receives on the socket. Preparation for _close call.
			_close();		/// Close the socket (+Cancel). Preparation for _release.
		}
		
		
		
		template class WSConnection<SSLSocket>;
		template class WSConnection<TCPSocket>;
		
		template<> template<>
		void WSConnection<TCPSocket>::Forward(uint& a)
		{
			stop();
		}
		
		template<> template<>
		void WSConnection<TCPSocket>::Forward(const std::string& a)
		{
			write(a);
		}
		
		template<> template<>
        void WSConnection<SSLSocket>::Forward(uint& a)
		{
			stop();
		}
		
		template<> template<>
		void WSConnection<SSLSocket>::Forward(const std::string& a)
		{
			write(a);
		}
	} // namespace WS
} // namespace net