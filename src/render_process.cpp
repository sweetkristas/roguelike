#include "engine.hpp"
#include "render_process.hpp"

namespace process
{
	render::render() 
		: process(ProcessPriority::render) 
	{
	}
	
	render::~render() 
	{
	}

	void render::update(engine& eng, double t, const std::vector<entity_ptr>& elist)
	{
		static component_id sprite_mask
			= (1 << component::Component::POSITION) 
			| (1 << component::Component::SPRITE);
		for(auto& e : elist) {
			if((e->get()->mask & sprite_mask) == sprite_mask) {
				auto& spr = e->get()->spr;
				auto& pos = e->get()->pos;
				if(spr->tex != NULL) {
					SDL_Rect dst = {pos->p.x * spr->width, pos->p.y * spr->height, spr->width, spr->height};
					SDL_RenderCopy(eng.get_renderer(), spr->tex, NULL, &dst);
				}
			}
		}
	}
}
