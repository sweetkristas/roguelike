#include "SDL.h"

#include "controller_process.hpp"
#include "engine.hpp"

namespace process
{
	controller::controller()
		: process(ProcessPriority::controller)
	{
	}

	controller::~controller()
	{
	}

	void controller::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
	}
}
