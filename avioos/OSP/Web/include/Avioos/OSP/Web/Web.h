
#ifndef Avioos_OSP_Web_Web_INCLUDED
#define Avioos_OSP_Web_Web_INCLUDED

#include "Poco/OSP/OSP.h"

#if defined(_WIN32) && defined(POCO_DLL)
	#if defined(AvioosOSPWeb_EXPORTS)
		#define AvioosOSPWeb_API __declspec(dllexport)
	#else
		#define AvioosOSPWeb_API __declspec(dllimport)
	#endif
#endif

#if !defined(AvioosOSPWeb_API)
	#define AvioosOSPWeb_API
#endif


#if defined(_MSC_VER)
	#if !defined(POCO_NO_AUTOMATIC_LIBS) && !defined(AvioosOSPWeb_EXPORTS)
		#pragma comment(lib, "AvioosOSPWeb" POCO_LIB_SUFFIX)
	#endif
#endif

#if defined(_MSC_VER)
	#pragma warning(disable:4503) // decorated name length exceeded, name was truncated
#endif

#endif // Avioos_OSP_Web_Web_INCLUDED