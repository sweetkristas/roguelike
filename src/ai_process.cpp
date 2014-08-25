#include <limits>
#include <random>

#include "ai_process.hpp"
#include "engine.hpp"

namespace process
{
	namespace
	{
		auto const seed = std::default_random_engine()();
		int get_random_int(int mn = std::numeric_limits<int>::min(), int mx = std::numeric_limits<int>::max())
		{
			static std::mt19937 random_engine(seed);
			std::uniform_int_distribution<int> uniform_dist(mn, mx);
			return uniform_dist(random_engine);
		}
	}

	ai::ai()
		: process(ProcessPriority::ai),
		  should_update_(false),
		  update_turns_(0)
	{
	}

	ai::~ai()
	{
	}

	bool ai::handle_event(const SDL_Event& evt)
	{
		switch(evt.type) {
			case SDL_USEREVENT:
				if(evt.user.code == static_cast<Sint32>(EngineUserEvents::NEW_TURN)) {
					should_update_ = true;
					update_turns_ = static_cast<int>(reinterpret_cast<intptr_t>(evt.user.data1));
				}
			default: break;
		}
		return false;
	}
	
	void ai::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		if(!should_update_) {
			return;
		}
		should_update_ = false;
		for(auto& e : elist) {
			static component_id ai_mask
				= (1 << component::Component::POSITION) 
				| (1 << component::Component::AI);

			if((e->get()->mask & ai_mask) == ai_mask) {
				auto& pos = e->get()->pos;
				//auto& aip = e->get()->aip;
			
				// XXX this should be rate limited a bit, so if the player wanted
				// to do something for 20 turns then we carry out 1 turn/200ms or so
				// then if the player needed to cancel action they could.
				// Fortunately we have a useful time parameter t to use.
				pos->last_p = pos->p;				
				update_turns_ = eng.get_turns() - update_turns_;
				for(int n = 0; n != update_turns_; ++n) {
					// XXX add some logic
					if(get_random_int(0,1)) {
						pos->p.x += get_random_int(-1, 1);
					} else {
						pos->p.y += get_random_int(-1, 1);
					}
				}
			}
		}
	}
}
