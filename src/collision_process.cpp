#include "engine.hpp"
#include "map.hpp"
#include "collision_process.hpp"

namespace process
{
	ee_collision::ee_collision()
		: process(ProcessPriority::collision)
	{
	
	}
	
	ee_collision::~ee_collision()
	{
	}

	// XXX Should we split this into a map/entity collision and entity/entity collision
	// classes?
	void ee_collision::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		static component_id collision_mask 
			= component::genmask(component::Component::POSITION)
			| component::genmask(component::Component::COLLISION);
		static component_id collision_map_mask = collision_mask | component::genmask(component::Component::MAP);
		// O(n^2) collision testing is for the pro's :-/
		// XXX Please make quad-tree or kd-tree for O(nlogn)
		for(auto& e1 : elist) {
			if((e1->get()->mask & collision_map_mask) == collision_mask) {
				auto& e1pos = e1->get()->pos;
				// XXX replace elist_ below with a call to eng.entities_in_area(r)
				// where r = active area for entities on map.
				for(auto& e2 : elist) {
					if(e1 == e2) {
						continue;
					}
					if((e2->get()->mask & collision_mask) == collision_mask) {
						// entity - entity collision
						auto& e2pos = e2->get()->pos;
						if(e1pos->pos+e1pos->mov != e2pos->pos) {
							e1pos->pos = e1pos->pos+e1pos->mov;
						} else {
							// XXX Not really sure I like this.
							// Basically clears the action to no action if there was a collision.
							// Needs to be applied for entity/map collision as well.
							using namespace component;
							if((e1->get()->mask & genmask(Component::INPUT)) == genmask(Component::INPUT)) {
								auto& inp = e1->get()->inp;
								inp->action = input::Action::none;
							}
						}
						e1pos->mov.clear();
					}
				}
			}
		}
	}

	em_collision::em_collision()
		: process(ProcessPriority::collision)
	{
	}
	
	em_collision::~em_collision()
	{
	}

	void em_collision::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		static component_id collision_mask 
			= component::genmask(component::Component::COLLISION)
			| component::genmask(component::Component::POSITION);
		static component_id  map_mask
			= component::genmask(component::Component::COLLISION)
			| component::genmask(component::Component::MAP);
		// O(n^2) collision testing is for the pro's :-/
		// XXX Please make quad-tree or kd-tree for O(nlogn)
		for(auto& e1 : elist) {
			if((e1->get()->mask & map_mask) == map_mask) {
				//auto& e1map = e1->get()->map;
				for(auto& e2 : elist) {
					if(e1 == e2) {
						continue;
					}
					if((e2->get()->mask & (collision_mask | (1 << component::Component::MAP))) == collision_mask) {
						//auto& e2pos = e2->get()->pos;
						// entity - map collision
						//if(!is_passable(e1map->map[e2pos->p.y][e2pos->p.x])) {
						//	e2pos->p = e2pos->last_p;
						//}
						//for(auto& exitp : e1map->exits) {
						//	if(e2pos->p == exitp) {
						//		if(e2->get()->is_player()) {
						//			// XXX do some exit logic, quitting the game for now
						//			eng.set_state(EngineState::QUIT);
						//		} else {
						//			// don't let the non-player controlled entities leave via exit
						//			e2pos->p = e2pos->last_p;
						//		}
						//	}
						//}
					} 
				}
			}
		}
	}
}
