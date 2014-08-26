#pragma once

#include <memory>

#include "entity.hpp"

class engine;

namespace process
{
	enum class ProcessPriority {
		input			= 100,
		ai				= 300,
		collision		= 700,
		action			= 800,
		gui			    = 850,
		render			= 900
	};

	// abstract system interface
	class process
	{
	public:
		explicit process(ProcessPriority priority);
		virtual ~process();
		virtual void start() {}
		virtual void end() {}
		virtual void update(engine& eng, double t, const std::vector<entity_ptr>& elist) = 0;

		bool process_event(const SDL_Event& evt);
		ProcessPriority get_priority() const { return priority_; }
	private:
		process();
		virtual bool handle_event(const SDL_Event& evt) { return false; }
		ProcessPriority priority_;
	};

	typedef std::shared_ptr<process> process_ptr;
}
