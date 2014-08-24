#pragma once

#include <memory>

#include "entity.hpp"

class engine;

namespace process
{
	enum class ProcessPriority {
		// controller is pretty much the highest of the high, since it let us pause the game,
		// process events, etc
		controller		= 1,
		input			= 100,
		movement		= 200,
		ai				= 300,
		collision		= 700,
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

	class movement : public process
	{
	public:
		movement();
		~movement();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	private:
	};
}
