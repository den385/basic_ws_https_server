#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"



namespace net
{
	namespace HTTP
	{
		//--------------------------------------------------------------------------------------------------------------
		// Cookie is a convenience struct storing all attributes of HTTP cookies and allowing to parse and unparse
		// cookies.
		//
		// Method 'toString' unparses a cookie with additional attributes (beside 'name' and 'value') into string,
		//   for 'set-cookie' header of HTTP response.
		// Method 'fromString' parses vector of cookies from string, coming in 'Cookie' HTTP header from client.
		//   The string looks like a list of name-value pairs.
		//
		// Fields 'http_only' and 'secure' are needed to cover vulnerabilities which come with the use of cookies.
		// Fields 'path' and 'domain' restrict usage of cookie to particular uri (so far unused).
		// Field 'expires' regulates how long browser will store this particular cookie coming in response from server.
		//
		// detailed info on security, including cookies (might be outdated):
		// -> https://phabricator.megaputer.ru/w/pa7/arch/security/exploits/
		//
		// detailed info on cookie format (might be outdated):
		// -> https://phabricator.megaputer.ru/w/pa7/webapi/lowlevelapi/cookie/
		//--------------------------------------------------------------------------------------------------------------
		struct WEBSERVER_API Cookie
		{
			Cookie() = default;
			Cookie(const std::string& n, const std::string& v, const std::string& p="", const std::string& d="",
				   bool h=false, bool s=false, double e=-1.0)
				: name(n), value(v), path(p), domain(d), http_only(h), secure(s), expires(e)
			{};
			
			bool empty() const;
			bool is_pauuid() const;
			
			// usage example in webserver/tests/cookie_test.cpp
			std::string toString() const;
			
			// usage example in webserver/tests/cookie_test.cpp
			static std::vector<Cookie> fromString(std::string s);
			
			static Cookie getInvalidationCookie(const std::string& name);
			
			std::string name;
			std::string value;
			
			std::string path;
			std::string domain;
			
			bool http_only = false;
			bool secure = false;
			
			double expires = -1.0;
		};
	}
}