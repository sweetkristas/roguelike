#include <algorithm>

#include "asserts.hpp"
#include "component.hpp"
#include "engine.hpp"
#include "profile_timer.hpp"

engine::engine(graphics::window_manager& wm)
	: wm_(wm),
	  turns_(1),
	  entity_quads_(0, rect(0,0,100,100)),
	  state_(EngineState::PLAY)
{
}

engine::~engine()
{
}

void engine::add_entity(entity_ptr e)
{
	entity_list_.emplace_back(e);
}

void engine::remove_entity(entity_ptr e1)
{
	entity_list_.erase(std::remove_if(entity_list_.begin(), entity_list_.end(), 
		[&e1](entity_ptr e2) { return e1 == e2; }), entity_list_.end());
}

void engine::add_process(process::process_ptr s)
{
	process_list_.emplace_back(s);
	std::stable_sort(process_list_.begin(), process_list_.end(), [](const process::process_ptr& lhs, const process::process_ptr& rhs){
		return lhs->get_priority() < rhs->get_priority();
	});
	s->start();
}

void engine::remove_process(process::process_ptr s)
{
	s->end();
	process_list_.erase(std::remove_if(process_list_.begin(), process_list_.end(), 
		[&s](process::process_ptr sp) { return sp == s; }), process_list_.end());
}

void engine::process_events()
{
	SDL_Event evt;
	while(SDL_PollEvent(&evt)) {
		bool claimed = false;
		switch(evt.type) {
			case SDL_MOUSEWHEEL:
				break;
			case SDL_QUIT:
				set_state(EngineState::QUIT);
				return;
			case SDL_KEYDOWN:
				if(evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					set_state(EngineState::QUIT);
					return;
				} else if(evt.key.keysym.scancode == SDL_SCANCODE_P) {
					if(SDL_GetModState() & KMOD_CTRL) {
						if(get_state() == EngineState::PLAY) {
							set_state(EngineState::PAUSE);
							LOG_INFO("Set state paused.");
						} else if(get_state() == EngineState::PAUSE) {
							set_state(EngineState::PLAY);
							LOG_INFO("Set state play.");
						}
						claimed = true;
					}
				}
				break;
		}
		if(!claimed) {
			for(auto& s : process_list_) {
				if(s->process_event(evt)) {
					break;
				}
			}
		}
	}
}

void engine::populate_quadtree()
{
	entity_quads_.clear();
	
	// only add entities to the quadtree that meet are collidable, but not maps
	static component_id collision_mask 
		= (1 << component::Component::POSITION)
		| (1 << component::Component::SPRITE)
		| (1 << component::Component::COLLISION);
	static component_id collision_map_mask = collision_mask | (1 << component::Component::MAP);

	for(auto& e : entity_list_) {
		if((e->get()->mask & collision_map_mask) == collision_mask) {
			auto& pos = e->get()->pos->p;
			auto& spr = e->get()->spr;
			entity_quads_.insert(e, rect(pos.x, pos.y, spr->width, spr->height));
		}
	}
}

std::vector<entity_ptr> engine::entities_in_area(const rect& r)
{
	std::vector<entity_ptr> res;
	entity_quads_.get_collidable(res, r);
	return res;
}

bool engine::update(double time)
{
	process_events();
	if(state_ == EngineState::PAUSE || state_ == EngineState::QUIT) {
		return state_ == EngineState::PAUSE ? true : false;
	}

	populate_quadtree();
	for(auto& p : process_list_) {
		p->update(*this, time, entity_list_);
	}

	return true;
}
