#pragma once

#ifdef WEBSERVER_EXPORTS
#define WEBSERVER_API EXPORT
#else
#define WEBSERVER_API IMPORT
#endif
