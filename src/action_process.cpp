#include "action_process.hpp"
#include "component.hpp"
#include "engine.hpp"

namespace process
{
	action::action()
		: process(ProcessPriority::action)
	{
	}

	action::~action()
	{
	}

	void action::update(engine& eng, double t, const entity_list& elist)
	{
		using namespace component;
		for(auto& e : elist) {
			if((e->mask & genmask(Component::INPUT)) == genmask(Component::INPUT)) {
				auto& inp = e->inp;
				switch (inp->action)
				{
				case input::Action::none:	break;
				case input::Action::moved:	
					if((e->mask & genmask(Component::POSITION)) == genmask(Component::POSITION)) {
						eng.set_camera(e->pos->pos);
					}
					break;
				case input::Action::pass:	break;
				case input::Action::attack:	break;
				case input::Action::spell:	break;
				case input::Action::use:	break;
				default: 
					ASSERT_LOG(false, "No action defined for " << static_cast<int>(inp->action));
					break;
				}
				// increment turns on successful update.
				if(inp->action != input::Action::none) {
					eng.inc_turns();
				}
			} else if((e->mask & genmask(Component::AI)) == genmask(Component::AI)) {
				//auto& aip = e->aip;
			}
		}
	}
}
