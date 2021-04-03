#pragma once

#include "webserver/expimp.h"
#include "webserver/stdhdr.h"



namespace net
{
	namespace HTTP
	{
		//--------------------------------------------------------------------------------------------------------------
		// This file is designed to store all constants of HTTP protocol in separate, often enclosed namespaces and in
		// good order. The idea is to avoid having protocol-related constants elsewhere.
		//--------------------------------------------------------------------------------------------------------------
		namespace Schema
		{
			// TODO: use lower-case headers only in response
			namespace Header
			{
				const std::string cookie =
					"cookie";
				const std::string set_cookie =
					"set-cookie";
				const std::string content_type =
					"Content-Type";
				const std::string last_modified =
					"Last-Modified";
				const std::string content_length =
					"Content-Length";
				const std::string connection =
					"Connection";
				const std::string host =
					"Host";
				
				namespace Value
				{
					const std::string keep_alive =
						"keep-alive";
					const std::string close =
						"close";
				}
				
				// Words start from upper-case letter by RFC
				// Still, some implementations of ws may use wrong casing
				namespace WS
				{
					const std::string upgrade =
						"upgrade";
					const std::string connection =
						"connection";
					const std::string sec_websocket_key =
						"sec-websocket-key";
					const std::string sec_websocket_version =
						"sec-websocket-version";
						
					namespace Value
					{
						const std::string websocket =
							"websocket";
						const std::string upgrade =
							"Upgrade";
						const std::string keep_alive_upgrade =
							"keep-alive, Upgrade";
					}
				}
			} // namespace Header
			
			
			
			// TODO: mv to webengine/web_env.h and pass this env to webserver
			namespace URI
			{
				const std::string WS_URLPREFIX =
					"/ws";
				const std::string WSS_URLPREFIX =
					"/wss";
				
				const std::string HTTP_PROTO =
					"http";
				const std::string HTTPS_PROTO =
					"https";
				const std::string WS_PROTO =
					"ws";
				const std::string WSS_PROTO =
					"wss";
				const std::string PROTO_URLPOSTFIX =
					"://";
			} // namespace URI
			
			
			
			enum StatusCode
			{
				undefined = 0,
				websocket_handshake = 101,
				ok = 200,
				created = 201,
				accepted = 202,
				no_content = 204,
				multiple_choices = 300,
				moved_permanently = 301,
				moved_temporarily = 302,
				not_modified = 304,
				bad_request = 400,
				unauthorized = 401,
				forbidden = 403,
				not_found = 404,
				internal_server_error = 500,
				not_implemented = 501,
				bad_gateway = 502,
				service_unavailable = 503
			};
			
			
			
			enum class ParserState
			{
				method_start,
				method,
				uri,
				http_version_h,
				http_version_t_1,
				http_version_t_2,
				http_version_p,
				http_version_slash,
				http_version_major_start,
				http_version_major,
				http_version_minor_start,
				http_version_minor,
				expecting_newline_1,
				header_line_start,
				header_lws,
				header_name,
				space_before_header_value,
				header_value,
				expecting_newline_2,
				expecting_newline_3,
				body
			};
			
			
			
			namespace StatusString
			{
				const std::string websocket_handshake =
					"HTTP/1.1 101 Switching Protocols\r\n";
				const std::string ok =
					"HTTP/1.0 200 OK\r\n";
				const std::string created =
					"HTTP/1.0 201 Created\r\n";
				const std::string accepted =
					"HTTP/1.0 202 Accepted\r\n";
				const std::string no_content =
					"HTTP/1.0 204 No Content\r\n";
				const std::string multiple_choices =
					"HTTP/1.0 300 Multiple Choices\r\n";
				const std::string moved_permanently =
					"HTTP/1.0 301 Moved Permanently\r\n";
				const std::string moved_temporarily =
					"HTTP/1.0 302 Moved Temporarily\r\n";
				const std::string not_modified =
					"HTTP/1.0 304 Not Modified\r\n";
				const std::string bad_request =
					"HTTP/1.0 400 Bad Request\r\n";
				const std::string unauthorized =
					"HTTP/1.0 401 Unauthorized\r\n";
				const std::string forbidden =
					"HTTP/1.0 403 Forbidden\r\n";
				const std::string not_found =
					"HTTP/1.0 404 Not Found\r\n";
				const std::string internal_server_error =
					"HTTP/1.0 500 Internal Server Error\r\n";
				const std::string not_implemented =
					"HTTP/1.0 501 Not Implemented\r\n";
				const std::string bad_gateway =
					"HTTP/1.0 502 Bad Gateway\r\n";
				const std::string service_unavailable =
					"HTTP/1.0 503 Service Unavailable\r\n";
			} // namespace StatusString
			
			
			
			namespace Magic
			{
				const char name_value_separator[] = { ':', ' ' };
				const char crlf[] = { '\r', '\n' };
				
				const std::string ws_token = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				const uint8_t ws_min_version = 13;
			} // namespace Magic
		
		
		
			namespace StockReply
			{
				const char ok[] = "";
				const char created[] =
						"<html>"
						"<head><title>Created</title></head>"
						"<body><h1>201 Created</h1></body>"
						"</html>";
				const char accepted[] =
						"<html>"
						"<head><title>Accepted</title></head>"
						"<body><h1>202 Accepted</h1></body>"
						"</html>";
				const char no_content[] =
						"<html>"
						"<head><title>No Content</title></head>"
						"<body><h1>204 Content</h1></body>"
						"</html>";
				const char multiple_choices[] =
						"<html>"
						"<head><title>Multiple Choices</title></head>"
						"<body><h1>300 Multiple Choices</h1></body>"
						"</html>";
				const char moved_permanently[] =
						"<html>"
						"<head><title>Moved Permanently</title></head>"
						"<body><h1>301 Moved Permanently</h1></body>"
						"</html>";
				const char moved_temporarily[] =
						"<html>"
						"<head><title>Moved Temporarily</title></head>"
						"<body><h1>302 Moved Temporarily</h1></body>"
						"</html>";
				const char not_modified[] =
						"<html>"
						"<head><title>Not Modified</title></head>"
						"<body><h1>304 Not Modified</h1></body>"
						"</html>";
				const char bad_request[] =
						"<html>"
						"<head><title>Bad Request</title></head>"
						"<body><h1>400 Bad Request</h1></body>"
						"</html>";
				const char unauthorized[] =
						"<html>"
						"<head><title>Unauthorized</title></head>"
						"<body><h1>401 Unauthorized</h1></body>"
						"</html>";
				const char forbidden[] =
						"<html>"
						"<head><title>Forbidden</title></head>"
						"<body><h1>403 Forbidden</h1></body>"
						"</html>";
				const char not_found[] =
						"<html>"
						"<head><title>Not Found</title></head>"
						"<body><h1>404 Not Found</h1></body>"
						"</html>";
				const char internal_server_error[] =
						"<html>"
						"<head><title>Internal Server Error</title></head>"
						"<body><h1>500 Internal Server Error</h1></body>"
						"</html>";
				const char not_implemented[] =
						"<html>"
						"<head><title>Not Implemented</title></head>"
						"<body><h1>501 Not Implemented</h1></body>"
						"</html>";
				const char bad_gateway[] =
						"<html>"
						"<head><title>Bad Gateway</title></head>"
						"<body><h1>502 Bad Gateway</h1></body>"
						"</html>";
				const char service_unavailable[] =
						"<html>"
						"<head><title>Service Unavailable</title></head>"
						"<body><h1>503 Service Unavailable</h1></body>"
						"</html>";
			} // namespace StockReply
			
			
			
			namespace MIME
			{
				using MAP = std::map<const std::string, const std::string>;
				
				const MAP Type =
				{
					{".a",       "application/octet-stream"},
					{".aab",     "application/x-authorware-bin"},
					{".aam",     "application/x-authorware-map"},
					{".aas",     "application/x-authorware-seg"},
					{".ai",      "application/postscript"},
					{".aif",     "audio/x-aiff"},
					{".aifc",    "audio/x-aiff"},
					{".aiff",    "audio/x-aiff"},
					{".asc",     "text/plain"},
					{".asf",     "video/x-ms-asf"},
					{".asx",     "video/x-ms-asf"},
					{".au",      "audio/basic"},
					{".avi",     "video/x-msvideo"},
					{".bcpio",   "application/x-bcpio"},
					{".bin",     "application/octet-stream"},
					{".bmp",     "image/bmp"},
					{".cdf",     "application/x-netcdf"},
					{".class",   "application/x-java-vm"},
					{".cpio",    "application/x-cpio"},
					{".cpt",     "application/mac-compactpro"},
					{".crl",     "application/x-pkcs7-crl"},
					{".crt",     "application/x-x509-ca-cert"},
					{".csh",     "application/x-csh"},
					{".css",     "text/css"},
					{".dcr",     "application/x-director"},
					{".dir",     "application/x-director"},
					{".djv",     "image/vnd.djvu"},
					{".djvu",    "image/vnd.djvu"},
					{".dll",     "application/octet-stream"},
					{".dms",     "application/octet-stream"},
					{".doc",     "application/msword"},
					{".dtd",     "text/xml"},
					{".dump",    "application/octet-stream"},
					{".dvi",     "application/x-dvi"},
					{".dxr",     "application/x-director"},
					{".eps",     "application/postscript"},
					{".etx",     "text/x-setext"},
					{".exe",     "application/octet-stream"},
					{".ez",      "application/andrew-inset"},
					{".fgd",     "application/x-director"},
					{".fh",      "image/x-freehand"},
					{".fh4",     "image/x-freehand"},
					{".fh5",     "image/x-freehand"},
					{".fh7",     "image/x-freehand"},
					{".fhc",     "image/x-freehand"},
					{".gif",     "image/gif"},
					{".gtar",    "application/x-gtar"},
					{".gz",      "application/x-gzip"},
					{".hdf",     "application/x-hdf"},
					{".hqx",     "application/mac-binhex40"},
					{".htm",     "text/html"},
					{".html",    "text/html"},
					{".ice",     "x-conference/x-cooltalk"},
					{".ief",     "image/ief"},
					{".iges",    "model/iges"},
					{".igs",     "model/iges"},
					{".iv",      "application/x-inventor"},
					{".jar",     "application/x-java-archive"},
					{".jfif",    "image/jpeg"},
					{".jpe",     "image/jpeg"},
					{".jpeg",    "image/jpeg"},
					{".jpg",     "image/jpeg"},
					{".js",      "application/javascript"},
					{".json",    "application/json"},
					{".kar",     "audio/midi"},
					{".latex",   "application/x-latex"},
					{".lha",     "application/octet-stream"},
					{".lzh",     "application/octet-stream"},
					{".m3u",     "audio/x-mpegurl"},
					{".man",     "application/x-troff-man"},
					{".mathml",  "application/mathml+xml"},
					{".me",      "application/x-troff-me"},
					{".mesh",    "model/mesh"},
					{".mid",     "audio/midi"},
					{".midi",    "audio/midi"},
					{".mif",     "application/vnd.mif"},
					{".mime",    "message/rfc822"},
					{".mml",     "application/mathml+xml"},
					{".mov",     "video/quicktime"},
					{".movie",   "video/x-sgi-movie"},
					{".mp2",     "audio/mpeg"},
					{".mp3",     "audio/mpeg"},
					{".mp4",     "video/mp4"},
					{".mpe",     "video/mpeg"},
					{".mpeg",    "video/mpeg"},
					{".mpg",     "video/mpeg"},
					{".mpga",    "audio/mpeg"},
					{".ms",      "application/x-troff-ms"},
					{".msh",     "model/mesh"},
					{".mv",      "video/x-sgi-movie"},
					{".mxu",     "video/vnd.mpegurl"},
					{".nc",      "application/x-netcdf"},
					{".o",       "application/octet-stream"},
					{".oda",     "application/oda"},
					{".ogg",     "audio/ogg"},
					{".pac",     "application/x-ns-proxy-autoconfig"},
					{".pbm",     "image/x-portable-bitmap"},
					{".pdb",     "chemical/x-pdb"},
					{".pdf",     "application/pdf"},
					{".pgm",     "image/x-portable-graymap"},
					{".pgn",     "application/x-chess-pgn"},
					{".png",     "image/png"},
					{".pnm",     "image/x-portable-anymap"},
					{".ppm",     "image/x-portable-pixmap"},
					{".ppt",     "application/vnd.ms-powerpoint"},
					{".ps",      "application/postscript"},
					{".qt",      "video/quicktime"},
					{".ra",      "audio/x-realaudio"},
					{".ram",     "audio/x-pn-realaudio"},
					{".ras",     "image/x-cmu-raster"},
					{".rdf",     "application/rdf+xml"},
					{".rgb",     "image/x-rgb"},
					{".rm",      "audio/x-pn-realaudio"},
					{".roff",    "application/x-troff"},
					{".rpm",     "audio/x-pn-realaudio-plugin"},
					{".rss",     "application/rss+xml"},
					{".rtf",     "text/rtf"},
					{".rtx",     "text/richtext"},
					{".sgm",     "text/sgml"},
					{".sgml",    "text/sgml"},
					{".sh",      "application/x-sh"},
					{".shar",    "application/x-shar"},
					{".silo",    "model/mesh"},
					{".sit",     "application/x-stuffit"},
					{".skd",     "application/x-koan"},
					{".skm",     "application/x-koan"},
					{".skp",     "application/x-koan"},
					{".skt",     "application/x-koan"},
					{".smi",     "application/smil"},
					{".smil",    "application/smil"},
					{".snd",     "audio/basic"},
					{".so",      "application/octet-stream"},
					{".spl",     "application/x-futuresplash"},
					{".src",     "application/x-wais-source"},
					{".stc",     "application/vnd.sun.xml.calc.template"},
					{".std",     "application/vnd.sun.xml.draw.template"},
					{".sti",     "application/vnd.sun.xml.impress.template"},
					{".stw",     "application/vnd.sun.xml.writer.template"},
					{".sv4cpio", "application/x-sv4cpio"},
					{".sv4crc",  "application/x-sv4crc"},
					{".svg",     "image/svg+xml"},
					{".svgz",    "image/svg+xml"},
					{".swf",     "application/x-shockwave-flash"},
					{".sxc",     "application/vnd.sun.xml.calc"},
					{".sxd",     "application/vnd.sun.xml.draw"},
					{".sxg",     "application/vnd.sun.xml.writer.global"},
					{".sxi",     "application/vnd.sun.xml.impress"},
					{".sxm",     "application/vnd.sun.xml.math"},
					{".sxw",     "application/vnd.sun.xml.writer"},
					{".t",       "application/x-troff"},
					{".tar",     "application/x-tar"},
					{".tcl",     "application/x-tcl"},
					{".tex",     "application/x-tex"},
					{".texi",    "application/x-texinfo"},
					{".texinfo", "application/x-texinfo"},
					{".tif",     "image/tiff"},
					{".tiff",    "image/tiff"},
					{".tr",      "application/x-troff"},
					{".tsp",     "application/dsptype"},
					{".tsv",     "text/tab-separated-values"},
					{".txt",     "text/plain"},
					{".ustar",   "application/x-ustar"},
					{".vcd",     "application/x-cdlink"},
					{".uu",      "text/x-uuencode"},
					{".vrml",    "model/vrml"},
					{".vx",      "video/x-rad-screenplay"},
					{".wav",     "audio/x-wav"},
					{".wax",     "audio/x-ms-wax"},
					{".wbmp",    "image/vnd.wap.wbmp"},
					{".wbxml",   "application/vnd.wap.wbxml"},
					{".wm",      "video/x-ms-wm"},
					{".wma",     "audio/x-ms-wma"},
					{".wmd",     "application/x-ms-wmd"},
					{".wml",     "text/vnd.wap.wml"},
					{".wmlc",    "application/vnd.wap.wmlc"},
					{".wmls",    "text/vnd.wap.wmlscript"},
					{".wmlsc",   "application/vnd.wap.wmlscriptc"},
					{".wmv",     "video/x-ms-wmv"},
					{".wmx",     "video/x-ms-wmx"},
					{".wmz",     "application/x-ms-wmz"},
					{".wrl",     "model/vrml"},
					{".wsrc",    "application/x-wais-source"},
					{".wvx",     "video/x-ms-wvx"},
					{".xbm",     "image/x-xbitmap"},
					{".xht",     "application/xhtml+xml"},
					{".xhtml",   "application/xhtml+xml"},
					{".xls",     "application/vnd.ms-excel"},
					{".xml",     "text/xml"},
					{".xpm",     "image/x-xpixmap"},
					{".xsl",     "text/xml"},
					{".xwd",     "image/x-xwindowdump"},
					{".xyz",     "chemical/x-xyz"},
					{".zip",     "application/zip"},
				};
				
				const std::string json = Type.at(".json");
				const std::string html = Type.at(".html");
				const std::string txt = Type.at(".txt");
				
				inline void augment_content_type(std::string& content_type)
				{
					if (content_type.find("text/") != std::string::npos)
					{
						content_type += "; charset=utf-8";
					}
				}
				
				inline std::string getContentType(const std::string& ext)
				{
					auto it = Type.find(ext);
			
					return (it == Type.end() ? txt : it->second);
				}
			} // namespace MIME
		} // namespace Schema
	} // namespace HTTP
} // namespace net