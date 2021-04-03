#include "webserver/stdafx.h"

#include "webserver/HTTP/http_request.h"
#include "webserver/HTTP/http_protocol.h"



namespace net
{
	namespace HTTP
	{
		Cookie HTTPRequest::getCookie(const std::string& name)
		{
			auto it = cookies.find(name);
			
			if (it != cookies.end())
				return it->second;
			else
				return Cookie();
		}

		bool HTTPRequest::isWSUpgrade()
		{
			namespace WSHeader = Schema::Header::WS;
			
			if (headers.find(WSHeader::upgrade) == headers.end() ||
				headers.find(WSHeader::connection) == headers.end())
				return false;
			
			return (lexcmpi(headers[WSHeader::upgrade], WSHeader::Value::websocket) == 0 &&
			       (lexcmpi(headers[WSHeader::connection], WSHeader::Value::upgrade) == 0 ||
			        lexcmpi(headers[WSHeader::connection], WSHeader::Value::keep_alive_upgrade) == 0));
		}
		
		bool HTTPRequest::isWSHandshake()
		{
			namespace WSHeader = Schema::Header::WS;
			
			if (!isWSUpgrade() || !(uri.scheme() == Schema::URI::WS_PROTO || uri.scheme() == Schema::URI::WSS_PROTO))
				return false;
			
			if (headers.find(WSHeader::sec_websocket_key) == headers.end() ||
				headers.find(WSHeader::sec_websocket_version) == headers.end())
				return false;
			
			return (!headers[WSHeader::sec_websocket_key].empty() &&
			        std::stoi(headers[WSHeader::sec_websocket_version]) >= 13);
		}
		
		void HTTPRequest::assembleUri(const std::string& uri_path_fragment)
		{
			std::string s_uri;
			
			s_uri += (isWSUpgrade() ? (is_http ? Schema::URI::WS_PROTO : Schema::URI::WSS_PROTO) :
			                          (is_http ? Schema::URI::HTTP_PROTO : Schema::URI::HTTPS_PROTO));
			s_uri += Schema::URI::PROTO_URLPOSTFIX;
			
			s_uri += headers.at(utflower(Schema::Header::host));						// may throw std::out_of_range
			s_uri += uri_path_fragment;
			
			uri = Uri(s_uri);
		}
	} // namespace HTTP
} // namespace net