#include "action_proces.hpp"
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

	void action::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		using namespace component;
		for(auto& e : elist) {
			if((e->get()->mask & genmask(Component::INPUT)) == genmask(Component::INPUT)) {
				auto& inp = e->get()->inp;
				switch (inp->action)
				{
				case input::Action::none:	break;
				case input::Action::moved:	break;
				case input::Action::pass:	break;
				case input::Action::attack:	break;
				case input::Action::spell:	break;
				case input::Action::use:	break;
				default: 
					ASSERT_LOG(false, "No action defined for " << static_cast<int>(inp->action));
					break;
				}
				// XXX increment turns on successful update.
				if(inp->action != input::Action::none) {
					eng.inc_turns();
				}
			} else if((e->get()->mask & genmask(Component::AI)) == genmask(Component::AI)) {
				//auto& aip = e->get()->aip;
			}
		}
	}
}
