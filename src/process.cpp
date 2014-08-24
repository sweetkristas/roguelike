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

	movement::movement()
		: process(ProcessPriority::movement)
	{
	}

	movement::~movement()
	{
	}

	void movement::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		static component_id movement_mask
			= (1 << component::Component::POSITION) 
			| (1 << component::Component::INPUT);

		for(auto& e : elist) {
			if((e->get()->mask & movement_mask) == movement_mask) {
				auto& inp = e->get()->inp;
				auto& pos = e->get()->pos;
				pos->last_p = pos->p;
				switch(inp->action)
				{
					case component::input::Action::none:	break;
					case component::input::Action::up:		pos->p.y -= 1; break;
					case component::input::Action::down:	pos->p.y += 1; break;
					case component::input::Action::left:	pos->p.x -= 1; break;
					case component::input::Action::right:	pos->p.x += 1; break;
					case component::input::Action::use:		break;
					case component::input::Action::attack:	break;
					default:
						ASSERT_LOG(false, "Unhandled case: " << static_cast<int>(inp->action));
				}
				if(pos->last_p != pos->p) {
					eng.inc_turns(); 
				}
			}
		}
	}
}
