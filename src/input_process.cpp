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
			switch(evt.key.keysym.scancode) {
			case SDL_SCANCODE_RIGHT:
			case SDL_SCANCODE_LEFT:
			case SDL_SCANCODE_DOWN:
			case SDL_SCANCODE_UP:
				keys_pressed_.push(evt.key.keysym.scancode);
				return true;
			default: break;
			}
		}
		return false;
	}

	void input::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		static component_id input_mask
			= (1 << component::Component::PLAYER) 
			| (1 << component::Component::INPUT);
		for(auto& e : elist) {

			if((e->get()->mask & input_mask) == input_mask) {
				auto& inp = e->get()->inp;
				inp->action = component::input::Action::none;
				if(!keys_pressed_.empty()) {
					auto key = keys_pressed_.front();
					keys_pressed_.pop();
					if(key == SDL_SCANCODE_LEFT) {
						inp->action = component::input::Action::left;
					} else if(key == SDL_SCANCODE_RIGHT) {
						inp->action = component::input::Action::right;
					} else if(key == SDL_SCANCODE_UP) {
						inp->action = component::input::Action::up;
					} else if(key == SDL_SCANCODE_DOWN) {
						inp->action = component::input::Action::down;
					}
				}
			}
		}
	}
}
