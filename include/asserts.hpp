#pragma once

#include <iostream>
#include <sstream>

#include "SDL.h"

#define ASSERT_LOG(_a,_b)																\
	do {																				\
		if(!(_a)) {																		\
			std::ostringstream _s;														\
			_s << __FILE__ << ":" << __LINE__ << " : " << _b;							\
			SDL_Log("%s\n", _s.str().c_str());											\
			exit(1);																	\
		}																				\
	} while(0)
