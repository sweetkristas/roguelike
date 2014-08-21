#pragma once
#ifndef PROFILE_TIMER_HPP_INCLUDED
#define PROFILE_TIMER_HPP_INCLUDED

#include "SDL.h"
#include <iostream>
#include <string>

#if SDL_VERSION_ATLEAST(2,0,0)
namespace profile 
{
	struct manager
	{
		Uint64 frequency;
		Uint64 t1, t2;
		double elapsedTime;
		const char* name;

		manager(const char* const str) : name(str)
		{
			frequency = SDL_GetPerformanceFrequency();
			t1 = SDL_GetPerformanceCounter();
		}

		~manager()
		{
			t2 = SDL_GetPerformanceCounter();
			elapsedTime = (t2 - t1) * 1000.0 / frequency;
			std::cerr << name << ": " << elapsedTime << " milliseconds" << std::endl;
		}
	};
}
#else
namespace profile
{
	struct manager
	{
		manager()  {}
		~manager() {}
	};
}
#endif

#endif 
