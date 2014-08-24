#include <algorithm>

#include "asserts.hpp"
#include "engine.hpp"
#include "profile_timer.hpp"

engine::engine(graphics::window_manager& wm)
	: wm_(wm),
	  turns_(1),
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

bool engine::update(double time)
{
	process_events();
	if(state_ == EngineState::PAUSE || state_ == EngineState::QUIT) {
		return state_ == EngineState::PAUSE ? true : false;
	}

	for(auto& p : process_list_) {
		p->update(*this, time, entity_list_);
	}

	return true;
}
