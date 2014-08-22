#pragma once

#include <iostream>
#include <sstream>

#include "SDL.h"

#if defined(_MSC_VER)
#define __SHORT_FORM_OF_FILE__		\
	(strrchr(__FILE__, '\\')		\
	? strrchr(__FILE__, '\\') + 1	\
	: __FILE__						\
	)
#else
#define __SHORT_FORM_OF_FILE__		\
	(strrchr(__FILE__, '/')			\
	? strrchr(__FILE__, '/') + 1	\
	: __FILE__						\
	)
#endif

#define ASSERT_LOG(_a,_b)															\
	do {																			\
		if(!(_a)) {																	\
			std::ostringstream _s;													\
			_s << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << " : " << _b;			\
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s\n", _s.str().c_str());\
			exit(1);																\
		}																			\
	} while(0)

#define LOG_INFO(_a)																\
	do {																			\
		std::ostringstream _s;														\
		_s << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << " : " << _a;				\
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s\n", _s.str().c_str());		\
	} while(0)

#define LOG_DEBUG(_a)																\
	do {																			\
		std::ostringstream _s;														\
		_s << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << " : " << _a;				\
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s\n", _s.str().c_str());		\
	} while(0)

#define LOG_WARN(_a)																\
	do {																			\
		std::ostringstream _s;														\
		_s << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << " : " << _a;				\
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s\n", _s.str().c_str());		\
	} while(0)

#define LOG_ERROR(_a)																\
	do {																			\
		std::ostringstream _s;														\
		_s << __SHORT_FORM_OF_FILE__ << ":" << __LINE__ << " : " << _a;				\
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", _s.str().c_str());		\
	} while(0)
