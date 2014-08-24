#include "engine.hpp"
#include "map.hpp"
#include "collision_process.hpp"

namespace process
{
	collision::collision()
		: process(ProcessPriority::collision)
	{
	
	}
	
	collision::~collision()
	{
	}

	// XXX Should we split this into a map/entity collision and entity/entity collision
	// classes?
	void collision::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		static component_id collision_mask
			= (1 << component::Component::COLLISION) 
			| (1 << component::Component::POSITION);
		static component_id  map_mask
			= (1 << component::Component::COLLISION) 
			| (1 << component::Component::MAP);
		// O(n^2) collision testing is for the pro's :-/
		// XXX Please make quad-tree or kd-tree for O(nlogn)
		for(auto& e1 : elist) {
			if((e1->get()->mask & collision_mask) == collision_mask && (e1->get()->mask & (1 << component::Component::MAP)) == 0) {
				auto& e1pos = e1->get()->pos;
				for(auto& e2 : elist) {
					if(e1 == e2) {
						continue;
					}
					if((e2->get()->mask & map_mask) == map_mask) {
						// entity - map collision
						auto& e2map = e2->get()->map;
						if(!is_passable(e2map->map[e1pos->p.y][e1pos->p.x])) {
							e1pos->p = e1pos->last_p;
						}
						for(auto& exitp : e2map->exits) {
							if(e1pos->p == exitp) {
								// XXX do some exit logic
								eng.set_state(EngineState::QUIT);
							}
						}
					} else if((e2->get()->mask & collision_mask) == collision_mask) {
						// entity - entity collision
						auto& e2pos = e2->get()->pos;
						if(e1pos->p == e2pos->p) {
							e1pos->p = e1pos->last_p;
						}
					}
				}
			}
		}
	}
}
