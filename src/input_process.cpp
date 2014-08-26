#include "input_process.hpp"

namespace process
{
	input::input()
		: process(ProcessPriority::input)
	{
	}

	input::~input()
	{
	}

	bool input::handle_event(const SDL_Event& evt)
	{
		if(evt.type == SDL_KEYDOWN) {
			keys_pressed_.push(evt.key.keysym.scancode);
			return true;
		}
		return false;
	}

	void input::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		static component_id input_mask
			= (1 << component::Component::POSITION) 
			| (1 << component::Component::INPUT);
		for(auto& e : elist) {

			if((e->get()->mask & input_mask) == input_mask) {
				auto& inp = e->get()->inp;
				auto& pos = e->get()->pos;
				inp->action = component::input::Action::none;
				if(!keys_pressed_.empty()) {
					auto key = keys_pressed_.front();
					keys_pressed_.pop();
					if(key == SDL_SCANCODE_LEFT) {
						pos->mov.x -= 1;
						inp->action = component::input::Action::moved;
					} else if(key == SDL_SCANCODE_RIGHT) {
						pos->mov.x += 1;
						inp->action = component::input::Action::moved;
					} else if(key == SDL_SCANCODE_UP) {
						pos->mov.y -= 1;
						inp->action = component::input::Action::moved;
					} else if(key == SDL_SCANCODE_DOWN) {
						pos->mov.y += 1;
						inp->action = component::input::Action::moved;
					} else if(key == SDL_SCANCODE_PERIOD) {
						inp->action = component::input::Action::pass;
					}
				}
			}
		}
	}
}
