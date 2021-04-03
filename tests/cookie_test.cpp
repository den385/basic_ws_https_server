#include "webserver/stdafx.h"

#include "core/test_engine/test_manager.h"
#include "webserver/HTTP/cookie.h"
#include "datatypes/converter.h"

using namespace units;
using namespace net;


void cookies_from_string()
{
	std::cout << "+++++++++++++ Testing creation of cookie from string ++++++++++++++++" << std::endl;
	
	std::string s = "CP=H2; "
		"WMF-Last-Access=09-Mar-2018; "
		"WMF-Last-Access-Global=09-Mar-2018; "
		"GeoIP=RU:MOW:Moscow:55.75:37.62:v4; "
		"Some.Var.1=42; "
		"Another_Var_2=!@#$%^&*()";
	
	auto vc = HTTP::Cookie::fromString(s);
	
	PA_ASSERT(vc.size() == 6);
	
	PA_ASSERT(vc[0].name == "CP" && vc[0].value == "H2");
	PA_ASSERT(vc[1].name == "WMF-Last-Access" && vc[1].value == "09-Mar-2018");
	PA_ASSERT(vc[2].name == "WMF-Last-Access-Global" && vc[2].value == "09-Mar-2018");
	PA_ASSERT(vc[3].name == "GeoIP" && vc[3].value == "RU:MOW:Moscow:55.75:37.62:v4");
	PA_ASSERT(vc[4].name == "Some.Var.1" && vc[4].value == "42");
	PA_ASSERT(vc[5].name == "Another_Var_2" && vc[5].value == "!@#$%^&*()");
	
	std::cout << "------------- Finished testing creation of cookie from string -------" << std::endl;
}

REGISTER_TEST("webserver/tests/cookies_from_string", cookies_from_string);



void cookie_to_string()
{
	std::cout << "+++++++++++++ Testing creation of string from cookie ++++++++++++++++" << std::endl;
	
	std::string etalon = "WMF-Last-Access-Global=09-Mar-2018; Path=/; Domain=.wikipedia.org; HttpOnly; Secure; Expires=Tue, 10 Apr 2018 12:00:00 GMT";
	
	ConverterSettings converter_settings;
	converter_settings.date_format = "DMY";
	Converter converter(converter_settings);
	double t = converter.StringToDateTime("Tue, 10 Apr 2018 12:00:00 GMT");
	
	HTTP::Cookie c("WMF-Last-Access-Global", "09-Mar-2018", "/", ".wikipedia.org", true, true, t);
	
	std::string result = c.toString();
	
	PA_ASSERT(result == etalon);
	
	std::cout << "------------- Finished testing creation of string from cookie -------" << std::endl;
}

REGISTER_TEST("webserver/tests/cookie_to_string", cookie_to_string);