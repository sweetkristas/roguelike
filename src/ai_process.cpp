#include "ai_process.hpp"

namespace process
{
	ai::ai()
		: process(ProcessPriority::ai)
	{
	}

	ai::~ai()
	{
	}
	
	void ai::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		for(auto& e : elist) {
			static component_id ai_mask
				= (1 << component::Component::POSITION) 
				| (1 << component::Component::AI);

			if((e->get()->mask & ai_mask) == ai_mask) {
				auto& pos = e->get()->pos;
				auto& aip = e->get()->ai;
			
				// XXX add some logic
			}
		}
	}
}
