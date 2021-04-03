#include "webserver/stdafx.h"

#include "webserver/HTTP/http_response.h"


namespace net
{
	namespace HTTP
	{	
		void HTTPResponse::setCookie(const HTTP::Cookie& c)
		{
			headers.insert({Schema::Header::set_cookie, c.toString()});
		}
		
		std::vector<boost::asio::const_buffer> HTTPResponse::to_buffers()
		{
			std::vector<boost::asio::const_buffer> buffers;
			buffers.push_back(status_str_to_buffer(status));
			
			for (auto it = headers.begin(); it != headers.end(); ++it)
			{
				buffers.push_back(boost::asio::buffer(it->first));
				buffers.push_back(boost::asio::buffer(Schema::Magic::name_value_separator));
				buffers.push_back(boost::asio::buffer(it->second));
				buffers.push_back(boost::asio::buffer(Schema::Magic::crlf));
			}
			
			buffers.push_back(boost::asio::buffer(Schema::Magic::crlf));
			buffers.push_back(boost::asio::buffer(content));
			
			return buffers;
		}
		
		boost::asio::const_buffer HTTPResponse::status_str_to_buffer(Schema::StatusCode status)
		{
			using namespace Schema::StatusString;
						
			switch (status)
			{
				case Schema::StatusCode::websocket_handshake:
					return boost::asio::buffer(websocket_handshake);
				case Schema::StatusCode::ok:
					return boost::asio::buffer(ok);
				case Schema::StatusCode::created:
					return boost::asio::buffer(created);
				case Schema::StatusCode::accepted:
					return boost::asio::buffer(accepted);
				case Schema::StatusCode::no_content:
					return boost::asio::buffer(no_content);
				case Schema::StatusCode::multiple_choices:
					return boost::asio::buffer(multiple_choices);
				case Schema::StatusCode::moved_permanently:
					return boost::asio::buffer(moved_permanently);
				case Schema::StatusCode::moved_temporarily:
					return boost::asio::buffer(moved_temporarily);
				case Schema::StatusCode::not_modified:
					return boost::asio::buffer(not_modified);
				case Schema::StatusCode::bad_request:
					return boost::asio::buffer(bad_request);
				case Schema::StatusCode::unauthorized:
					return boost::asio::buffer(unauthorized);
				case Schema::StatusCode::forbidden:
					return boost::asio::buffer(forbidden);
				case Schema::StatusCode::not_found:
					return boost::asio::buffer(not_found);
				case Schema::StatusCode::internal_server_error:
					return boost::asio::buffer(internal_server_error);
				case Schema::StatusCode::not_implemented:
					return boost::asio::buffer(not_implemented);
				case Schema::StatusCode::bad_gateway:
					return boost::asio::buffer(bad_gateway);
				case Schema::StatusCode::service_unavailable:
					return boost::asio::buffer(service_unavailable);
				default:
					return boost::asio::buffer(internal_server_error);
			}
		}
		
		HTTPResponse HTTPResponse::stock_reply(Schema::StatusCode status)
		{
			HTTPResponse rep;
			rep.status = status;
			rep.content = stock_reply_str(status);
			
			rep.headers["Content-Length"] = std::to_string(rep.content.size());
			rep.headers["Content-Type"] = "text/html";
			
			return rep;
		}
		
		std::string HTTPResponse::stock_reply_str(Schema::StatusCode status)
		{
			using namespace Schema::StockReply;
			
			switch (status)
			{
				case Schema::StatusCode::ok:
					return ok;
				case Schema::StatusCode::created:
					return created;
				case Schema::StatusCode::accepted:
					return accepted;
				case Schema::StatusCode::no_content:
					return no_content;
				case Schema::StatusCode::multiple_choices:
					return multiple_choices;
				case Schema::StatusCode::moved_permanently:
					return moved_permanently;
				case Schema::StatusCode::moved_temporarily:
					return moved_temporarily;
				case Schema::StatusCode::not_modified:
					return not_modified;
				case Schema::StatusCode::bad_request:
					return bad_request;
				case Schema::StatusCode::unauthorized:
					return unauthorized;
				case Schema::StatusCode::forbidden:
					return forbidden;
				case Schema::StatusCode::not_found:
					return not_found;
				case Schema::StatusCode::internal_server_error:
					return internal_server_error;
				case Schema::StatusCode::not_implemented:
					return not_implemented;
				case Schema::StatusCode::bad_gateway:
					return bad_gateway;
				case Schema::StatusCode::service_unavailable:
					return service_unavailable;
				default:
					return internal_server_error;
			}
		}
	} // namespace HTTP
} // namespace net