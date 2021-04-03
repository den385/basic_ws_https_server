#include "webserver/stdafx.h"

#include "webserver/HTTP/cookie.h"

#include <regex>



namespace net
{
	namespace HTTP
	{
		bool Cookie::empty() const
		{
			return (name.empty() || value.empty());
		}
		
		bool Cookie::is_pauuid() const
		{
			try { pauuid(value); }
			catch (boost::bad_lexical_cast&) { return false; } // exception, not pauuid string
			
			return true; // no exception, pauuid string, maybe empty (nil pauuid)
		}
		
		std::string Cookie::toString() const
		{
			if (name.empty() || value.empty())
				return "";
			
			std::stringstream ss;
			
			ss << name << "=" << value;
			
			if (!path.empty())
				ss << "; Path=" << path;
			
			if (!domain.empty())
				ss << "; Domain=" << domain;
			
			if (http_only)
				ss << "; HttpOnly";
			
			if (secure)
				ss << "; Secure";
			
			if (expires != -1.0)
				ss << "; Expires=" << PADateTime(expires).FormatDateTime(PADateTime::web_gmt_format);
			
			return ss.str();
		}
		
		std::vector<Cookie> Cookie::fromString(std::string s)
		{
			std::vector<Cookie> vc;
			
			std::regex cookie_regex(R"(([\w-_.]+)=([^;]*))");
			std::smatch cookie_match;
			
			while (std::regex_search(s, cookie_match, cookie_regex))
			{
				std::string cookie_name = cookie_match[1].str();
				std::string cookie_value = cookie_match[2].str();
	
				vc.emplace_back(Cookie(cookie_name, cookie_value));
	
				s = cookie_match.suffix();
			}
			
			return vc;
		}
		
		Cookie Cookie::getInvalidationCookie(const std::string& name)
		{
			Cookie ck(name, "#DELETED#" /* default attributes */);
			ck.expires = 0; // = Thu, 01 Jan 1970 00:00:00 GMT = magic date ("start of Unix epoch")
			
			return ck;
		}
	}
}