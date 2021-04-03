#include "webserver/stdafx.h"

#include "webserver/webserver.h"
#include "webserver/connection.h"
#include "webserver/HTTP/http_connection.h"
#include "openssl/ssl.h"



namespace net
{
	WebServer::WebServer(IOService& main_service, IOService& acceptor_service,
						 WebServerParams params, HTTP::HTTPRequestHandler::CreatorType http_bridge_creator)
	try : main_service_(main_service), // reuse_addr = true: do not swallow bind/listen error in socket
		  http_acceptor_(acceptor_service, NetEndpoint(tcp_flags::v4(), params.local_http_port), /* reuse_addr = */ true),
		  https_acceptor_(acceptor_service, NetEndpoint(tcp_flags::v4(), params.local_https_port), /* reuse_addr = */ true),
		  http_bridge_creator_(http_bridge_creator), params_(params)
	{
		context_ = std::make_shared<SSLContext>(acceptor_service, SSLContext::tlsv12);
		setup_ssl();
	}
	catch (system_error& e) // reuse_addr option may throw
	{
		IFLOG(P1, "Error in boost::asio acceptor creation. Error info (code + message) follows.", e.code());
	}
	
	void WebServer::Start()
	{
		do_accept_http();
		do_accept_https();
	}
	
	void WebServer::Stop()
	{
		error_code ec;
		
		http_acceptor_.close(ec);
		https_acceptor_.close(ec);
		
		if (ec)
			IFLOG(P2, "Exception happened while closing http(s) acceptor. Error info (code+message) follows.", ec);
	}
	
	bool WebServer::WSPush(const pauuid& conn_id, std::string const& s)
	{
		boost::lock_guard<ptl::mutex> lck(ws_conns_mx_);
		
		return (ws_forward(conn_id, ws_conns_, s) ||
		        ws_forward(conn_id, ws_secure_conns_, s));
	}
	
	bool WebServer::WSClose(const pauuid& conn_id, uint status_code)
	{
		boost::lock_guard<ptl::mutex> lck(ws_conns_mx_);
		
		return (ws_forward(conn_id, ws_conns_, status_code) ||         // first try to forward closure status into WS connection by id
		        ws_forward(conn_id, ws_secure_conns_, status_code));   // if failure, try to forward into WSS connection by id
	}
	
	
	
	// Forming of secure context
	// server.key - server private key (searched for beside pa7 configuration files)
	// server.crt - server certificate
	// see detailed cipher description on project wiki (might be outdated):
	// -> https://phabricator.megaputer.ru/w/pa7/webapi/lowlevelapi/ssl/
	void WebServer::setup_ssl()
	{
		context_->set_options(
			SSLContext::default_workarounds
			| SSLContext::no_sslv2
			| SSLContext::no_sslv3
			| SSLContext::no_tlsv1_1
			| SSL_OP_SINGLE_ECDH_USE
		);
		
		auto fpath = fs::GetProcessRootDirectory();
		
		if (fs::exists(fpath / "server.crt") && fs::exists(fpath / "server.key"))
		{
			try
			{
				context_->use_certificate_chain_file((fpath / "server.crt").string());
				context_->use_private_key_file((fpath / "server.key").string(), SSLContext::pem);
			
				std::unique_ptr<EC_KEY, void(*)(EC_KEY*)> ecdh(EC_KEY_new_by_curve_name(NID_X9_62_prime256v1), EC_KEY_free);
				
				SSL_CTX_set_tmp_ecdh(context_->native_handle(), ecdh.get());
				
				char* cipher_list = TLS1_TXT_ECDHE_RSA_WITH_AES_256_GCM_SHA384 " "
				                    TLS1_TXT_ECDHE_RSA_WITH_AES_128_GCM_SHA256 " "
				                    TLS1_TXT_RSA_WITH_AES_256_GCM_SHA384       " "
				                    TLS1_TXT_RSA_WITH_AES_128_GCM_SHA256;
				
				SSL_CTX_set_cipher_list(context_->native_handle(), cipher_list);
			}
			catch (std::exception &e)
			{
				IFLOG(P1, "SSL certificate and/or private key broken. SSL disabled. Security broken.", e.what());
			}
		}
		else
			IFLOG(P1, "SSL certificate and/or private key absent. SSL disabled. Security broken.");
		
		params_.context = context_;
	}
	
	template<typename TMap, typename TArg>
	bool WebServer::ws_forward(const pauuid& conn_id, TMap& ws_map, TArg& a)
	{
		bool ok = false;
		
		auto ws_conn = ws_map.find(conn_id);
		if (ws_conn != ws_map.end())
		{
			if (auto conn_lck = ws_conn->second.lock())
			{
				conn_lck->Forward(a);
				ok = true;
			}
			else
			{
				IFLOG(P4, "WS message push error. Connection handle dead. Erasing connnection with following id.", conn_id);
				ws_map.erase(ws_conn);
			}
		}
		
		return ok;
	}
	
	
	
	void WebServer::do_accept_http()
	{
		auto sock = std::make_shared<TCPSocket>(main_service_);
		http_acceptor_.async_accept(*sock.get(),
			[this, sock](error_code ec)
			{
				if (!http_acceptor_.is_open())
				{
					return;
				}

				if (!ec)
				{
					params_.remote_ip = sock->remote_endpoint().address();
					params_.remote_port = sock->remote_endpoint().port();

					auto conn = std::make_shared<HTTP::HTTPConnection<TCPSocket>>(*this, sock, params_, http_bridge_creator_());
					conn->Start();
				}

				do_accept_http();  // process next http connection
			}
		);
	}
	
	void WebServer::do_accept_https()
	{
		auto ssl_sock = std::make_shared<SSLSocket>(main_service_, *context_.get());
		https_acceptor_.async_accept(ssl_sock->lowest_layer(),
			[this, ssl_sock](boost::system::error_code ec)
			{
				if (!https_acceptor_.is_open())
				{
					return;
				}

				if (!ec)
				{
					params_.remote_ip = ssl_sock->lowest_layer().remote_endpoint().address();
					params_.remote_port = ssl_sock->lowest_layer().remote_endpoint().port();

					auto conn = std::make_shared<HTTP::HTTPConnection<SSLSocket>>(*this, ssl_sock, params_, http_bridge_creator_());
					conn->Start();
				}

				do_accept_https(); // process next https connection
			}
		);
	}
	
	
	
	template<>
	void WebServer::WSAddSession<TCPSocket>(const pauuid& id, std::weak_ptr<WS::WSConnection<TCPSocket>> wp)
	{
		boost::lock_guard<ptl::mutex> lck(ws_conns_mx_);
		
		ws_conns_.insert( { id, wp } );
	}
	
	template<>
	void WebServer::WSAddSession<SSLSocket>(const pauuid& id, std::weak_ptr<WS::WSConnection<SSLSocket>> wp)
	{
		boost::lock_guard<ptl::mutex> lck(ws_conns_mx_);
		
		ws_secure_conns_.insert( { id, wp } );
	}
	
	
	
	std::function<void(const pauuid&, const pauuid&, Uri const&)>     WebServer::ws_onopen        = nullptr;
	std::function<void(const pauuid&, std::string const&)>            WebServer::ws_onmessagein   = nullptr;
	std::function<void(const pauuid&, std::string const&)>            WebServer::ws_onmessageout  = nullptr;
	std::function<void(const pauuid&, error_code const&)>             WebServer::ws_onerror       = nullptr;
	std::function<void(const pauuid&, const Uri&)>                    WebServer::ws_onclose       = nullptr;
}