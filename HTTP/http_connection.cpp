#include "webserver/stdafx.h"

#include "webserver/HTTP/http_connection.h"
#include "webserver/webserver.h"
#include "webserver/WS/ws_connection.h"
#include "core/openssl_encoders.h"

#include <boost/bind.hpp>

#include <htmlcxx/html/Uri.h>

#include <type_traits>



namespace net
{
	namespace HTTP
	{
		template<typename TSocket>
		HTTPConnection<TSocket>::HTTPConnection(WebServer& webserver, std::shared_ptr<TSocket> sock, WebServerParams params,
			std::unique_ptr<HTTPRequestHandler> bridge)
		: Connection<TSocket>(sock, params), webserver_(webserver), bridge_(std::move(bridge)), strand_(sock->get_io_service())
		{
		}
		
		template<>
		void HTTPConnection<TCPSocket>::Start()
		{
			do_read();
		}
		
		template<>
		void HTTPConnection<SSLSocket>::Start()
		{
			
			auto self = this->shared_from_this();
			
			boost::asio::ip::tcp::no_delay option(true);
			sock_->lowest_layer().set_option(option);
			
			sock_.get()->async_handshake(
				boost::asio::ssl::stream_base::server,
				strand_.wrap([this, self](boost::system::error_code ec)
				{
					if (!ec)
					{
						do_read();
					}
				}
			));
		}
		
		
		
		template<typename TSocket>
		void HTTPConnection<TSocket>::do_read()
		{
			auto self = this->shared_from_this();
			sock_->async_read_some(boost::asio::buffer(buffer_),
				strand_.wrap([this, self](boost::system::error_code ec, std::size_t bytes_transferred)
				{
					if (!ec)
					{
						request_ = HTTPRequest();
						request_.is_http = is_http;
						request_.origin = params.remote_ip;
						
						response_ = HTTPResponse();
						
						auto result = request_parser_.Parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);
						
						if (result == HTTPParser::Result::good)
						{
							if (request_.isWSHandshake())
							{
								process_ws_handshake(request_.content);
							}
							else
							{
								try
								{
									bridge_->HandleRequest(request_, response_);
								}
								catch(std::exception& e)
								{
									IFLOG(P3, "HTTP request handling error, reason follows.", e.what());
								}
								
								do_write();
							}
						}
						else if (result == HTTPParser::Result::bad)
						{
							response_ = HTTPResponse::stock_reply(Schema::StatusCode::bad_request);
							
							do_write();
						}
						else
						{
							do_read();
						}
					}
					else if (ec != boost::asio::error::operation_aborted)
					{
						stop();
					}
				}
			));
		}
		
		template<typename TSocket>
		void HTTPConnection<TSocket>::do_write()
		{
			auto self = this->shared_from_this();
			boost::asio::async_write(*sock_.get(), response_.to_buffers(),
				strand_.wrap([this, self](boost::system::error_code ec, std::size_t)
				{
					if (!ec)
					{
						do_read();
					}
					else if (ec != boost::asio::error::operation_aborted)
					{
						stop();
					}
				}
			));
		}
		
		
		
		template<typename TSocket>
		void HTTPConnection<TSocket>::process_ws_handshake(const std::string &content)
		{
			auto self = this->shared_from_this();
		
			_cancel();
			
			_generate_ws_handshake_headers();
			
			auto buffers = response_.to_buffers();
			
			boost::asio::async_write(*sock_.get(), buffers,
				strand_.wrap(boost::bind(&HTTPConnection<TSocket>::_create_ws_connection, this, self)));
		}
		
		template<typename TSocket>
		void HTTPConnection<TSocket>::_generate_ws_handshake_headers()
		{
			std::string hash = base64SHA1(request_.headers["sec-websocket-key"] + Schema::Magic::ws_token);
			
			response_.status = Schema::StatusCode::websocket_handshake;
			response_.headers.insert(
				{
					{ "Upgrade", "websocket" },
					{ "Connection", "Upgrade" },
					{ "Sec-WebSocket-Accept", hash}
				}
			);
			
			if (request_.headers["sec-websocket-protocol"] == "chat, superchat")
				response_.headers["Sec-WebSocket-Protocol"] = "chat";
		}
		
		template<typename TSocket>
		void HTTPConnection<TSocket>::_create_ws_connection(std::shared_ptr<HTTPConnection<TSocket>> self)
		{
			auto id = uuid_generate();
			
			pauuid sid = request_.getCookie("SID").value;
			
			// after WSConnection "steals" 'sock_' socket of HTTPConnection, http connection is destroyed, because
			// no 'shared_from_this' is used for connection prolongation
			auto wsconn = std::make_shared<WS::WSConnection<TSocket>>(sock_, params, request_.uri, sid, id);
			
			std::weak_ptr<WS::WSConnection<TSocket>> wsconn_weak = wsconn;
			webserver_.WSAddSession(id, wsconn_weak);
			
			wsconn->Start();
		}
		
		
		
		template<typename TSocket>
		void HTTPConnection<TSocket>::stop()
		{
			_cancel();
			_shutdown();
			_close();
		}
		
		
		
		template class HTTPConnection<TCPSocket>;
		template class HTTPConnection<SSLSocket>;
	}
}