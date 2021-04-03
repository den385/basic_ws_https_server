#include "webserver/stdafx.h"

#include "webserver/HTTP/http_parser.h"
#include "webserver/HTTP/http_request.h"
#include "core/zeroout.h"
#include "webserver/HTTP/http_protocol.h"



namespace net
{
	namespace HTTP
	{
		HTTPParser::HTTPParser()
			: state_(Schema::ParserState::method_start)
		{
		}
		
		void HTTPParser::Reset()
		{
			state_ = Schema::ParserState::method_start;
			
			std::vector<char, ptl::SecureAlloc<char>> v;
			data_.content.swap(v);	// data_.content is zeroout-ed by custom allocator in v after stack frame is closed
			data_ = Data();
		}
		
		HTTPParser::Result HTTPParser::Parse(HTTPRequest& req, char* begin, char* end)
		{
			void* target_memory_ptr = reinterpret_cast<void*>(begin);
			size_t target_memory_sz = end - begin; // boost::asio buffer is a continuous memory chunk
			
			auto result = parse_impl(begin, end);
			
			if (result == Result::good) try {
				fill_request(req);
			} catch (std::out_of_range& e) { /// no 'Host' header
				result = Result::bad;
			}
			
			if (result != Result::indeterminate)
				Reset();
			
			zeroout(target_memory_ptr, target_memory_sz); // boost::asio buffer is zeroouted too
			
			return result;
		}
		
		HTTPParser::Result HTTPParser::parse_impl(char* begin, char* end)
		{
			using State = Schema::ParserState;
			
			while (begin != end)
			{
				char input = *begin;
				
				switch (state_)
				{
					case State::method_start:
						if (!is_char(input) || is_ctl(input) || is_tspecial(input))
						{
							return Result::bad;
						}
						else
						{
							state_ = State::method;
							data_.method.push_back(input);
						}
						break;
					case State::method:
						if (input == ' ')
						{
							state_ = State::uri;
						}
						else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
						{
							return Result::bad;
						}
						else
						{
							data_.method.push_back(input);
						}
						break;
					case State::uri:
						if (input == ' ')
						{
							state_ = State::http_version_h;
						}
						else if (is_ctl(input))
						{
							return Result::bad;
						}
						else
						{
							data_.uri.push_back(input);
						}
						break;
					case State::http_version_h:
						if (input == 'H')
						{
							state_ = State::http_version_t_1;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_t_1:
						if (input == 'T')
						{
							state_ = State::http_version_t_2;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_t_2:
						if (input == 'T')
						{
							state_ = State::http_version_p;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_p:
						if (input == 'P')
						{
							state_ = State::http_version_slash;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_slash:
						if (input == '/')
						{
							state_ = State::http_version_major_start;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_major_start:
						if (is_digit(input))
						{
							data_.http_version_major = data_.http_version_major * 10 + (input - '0');
							state_ = State::http_version_major;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_major:
						if (input == '.')
						{
							state_ = State::http_version_minor_start;
						}
						else if (is_digit(input))
						{
							data_.http_version_major = data_.http_version_major * 10 + (input - '0');
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_minor_start:
						if (is_digit(input))
						{
							data_.http_version_minor = data_.http_version_minor * 10 + (input - '0');
							state_ = State::http_version_minor;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::http_version_minor:
						if (input == '\r')
						{
							state_ = State::expecting_newline_1;
						}
						else if (is_digit(input))
						{
							data_.http_version_minor = data_.http_version_minor * 10 + (input - '0');
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::expecting_newline_1:
						if (input == '\n')
						{
							state_ = State::header_line_start;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::header_line_start:
						if (input == '\r')
						{
							state_ = State::expecting_newline_3;
						}
						else if (!data_.headers.empty() && (input == ' ' || input == '\t'))
						{
							state_ = State::header_lws;
						}
						else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
						{
							return Result::bad;
						}
						else
						{
							data_.headers.emplace_back(HTTPHeader());
							data_.headers.back().first.push_back(input);
							state_ = State::header_name;
						}
						break;
					case State::header_lws:
						if (input == '\r')
						{
							state_ = State::expecting_newline_2;
						}
						else if (input == ' ' || input == '\t')
							;
						else if (is_ctl(input))
						{
							return Result::bad;
						}
						else
						{
							state_ = State::header_value;
							data_.headers.back().second.push_back(input);
						}
						break;
					case State::header_name:
						if (input == ':')
						{
							state_ = State::space_before_header_value;
						}
						else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
						{
							return Result::bad;
						}
						else
						{
							data_.headers.back().first.push_back(input);
						}
						break;
					case State::space_before_header_value:
						if (input == ' ')
						{
							state_ = State::header_value;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::header_value:
						if (input == '\r')
						{
							state_ = State::expecting_newline_2;
						}
						else if (is_ctl(input))
						{
							return Result::bad;
						}
						else
						{
							data_.headers.back().second.push_back(input);
						}
						break;
					case State::expecting_newline_2:
						if (input == '\n')
						{
							state_ = State::header_line_start;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::expecting_newline_3:
						if (input == '\n')
						{
							state_ = State::body;
							
							for (auto& h : data_.headers)
								if (lexcmpi(h.first, "Content-Length") == 0)
								{
									data_.content_length = std::stoi(h.second);
									break;
								}
							
							if (data_.content_length == 0)					// no content
								return Result::good;
						}
						else
						{
							return Result::bad;
						}
						break;
					case State::body:
						data_.content.push_back(input);
						if (data_.content.size() == data_.content_length) // full content
							return Result::good;
						break;
					default:
						return Result::bad;
				}
				
				++begin;
			}
			
			return Result::indeterminate;
		}
		
		void HTTPParser::fill_request(HTTPRequest& req)
		{
			req.headers.clear();
			
			for (auto& h : data_.headers)
			{
				req.headers.insert({ utflower(h.first), h.second });
			}
			
			req.method = data_.method;
			req.http_version_major = data_.http_version_major;
			req.http_version_minor = data_.http_version_minor;
			
			// store cookies separately and in rich format
			if (req.headers.find(Schema::Header::cookie) != req.headers.end())
			{
				std::string cookies = req.headers[Schema::Header::cookie];
				std::vector<HTTP::Cookie> vc = HTTP::Cookie::fromString(cookies);
				
				for (auto& c : vc)
					req.cookies.insert({c.name, c});
				
				req.headers.erase(Schema::Header::cookie);
			}
			
			if (req.headers.find(utflower(Schema::Header::content_length)) != req.headers.end() &&
				data_.content.size() == data_.content_length)
				req.content.assign(data_.content.begin(), data_.content.end());
			
			// HTTP message does not carry Uri in full form
			req.assembleUri(data_.uri);
		};
		
		
		
		bool HTTPParser::is_char(int c)
		{
			return c >= 0 && c <= 127;
		}
		
		bool HTTPParser::is_ctl(int c)
		{
			return (c >= 0 && c <= 31) || (c == 127);
		}
		
		bool HTTPParser::is_tspecial(int c)
		{
			switch (c)
			{
				case '(': case ')': case '<': case '>': case '@':
				case ',': case ';': case ':': case '\\': case '"':
				case '/': case '[': case ']': case '?': case '=':
				case '{': case '}': case ' ': case '\t':
					return true;
				default:
					return false;
			}
		}
		
		bool HTTPParser::is_digit(int c)
		{
			return c >= '0' && c <= '9';
		}
	} // namespace HTTP
} // namespace net