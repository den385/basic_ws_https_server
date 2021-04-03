#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/buffer.hpp>

#include "core/web_utils/ip_address.h"
#include "core/web_utils/uri.h"
#include "templates/make_vector.h"



namespace net
{
	using boost::system::error_code;
	using boost::system::system_error;
	using boost::asio::error::basic_errors;
	
	using IOService = boost::asio::io_service;
	using NetEndpoint = boost::asio::ip::tcp::endpoint;
	
	using NetStreambuf = boost::asio::streambuf;
	using Strand = boost::asio::strand;
	using NetCBuffer = boost::asio::const_buffer;
	
	using boost::asio::async_read;
	using boost::asio::async_write;
	using boost::asio::transfer_exactly;
	
	using TCPAcceptor = boost::asio::ip::tcp::acceptor;
	using SSLSocket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
	using TCPSocket = boost::asio::ip::tcp::socket;
	using SSLContext = boost::asio::ssl::context;
	
	using Uri = pa::Uri;
	using IPAddress = pa::IPAddress;
	
	using tcp_flags = boost::asio::ip::tcp;
	
	namespace HTTP
	{
		using HTTPHeader = std::pair<std::string, std::string>; // TODO: make struct, augment with default headers
	}
}