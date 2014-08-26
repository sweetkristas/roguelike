#include "asserts.hpp"
#include "engine.hpp"
#include "process.hpp"

namespace process
{
	process::process(ProcessPriority priority)
		: priority_(priority)
	{
	}

	process::~process()
	{
	}

	bool process::process_event(const SDL_Event& evt)
	{
		return handle_event(evt);
	}
}
