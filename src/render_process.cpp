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
		static component_id gui_mask = sprite_mask | (1 << component::Component::GUI);
		static component_id map_mask
			= (1 << component::Component::MAP)
			| (1 << component::Component::SPRITE);
		
		const point& cam = eng.get_camera();
		const point screen_centre(eng.get_window().width() / 2, eng.get_window().height() / 2);
		const point& ts = eng.get_tile_size();

		for(auto& e : elist) {
			if((e->get()->mask & gui_mask) == gui_mask) {
				auto& spr = e->get()->spr;
				auto& pos = e->get()->pos;
				if(spr->tex != NULL) {
					SDL_Rect dst = {pos->p.x, pos->p.y, spr->width, spr->height};
					SDL_RenderCopy(eng.get_renderer(), spr->tex, NULL, &dst);
				}
			}  else if((e->get()->mask & sprite_mask) == sprite_mask) {
				auto& spr = e->get()->spr;
				auto& pos = e->get()->pos;
				if(spr->tex != NULL) {
					SDL_Rect dst = {screen_centre.x - (cam.x - pos->p.x) * ts.x, screen_centre.y - (cam.y - pos->p.y) * ts.y, spr->width, spr->height};
					SDL_RenderCopy(eng.get_renderer(), spr->tex, NULL, &dst);
				}
			} else if((e->get()->mask & map_mask) == map_mask) {
				auto& spr = e->get()->spr;
				if(spr->tex != NULL) {
					SDL_Rect dst = {screen_centre.x - cam.x * ts.x, screen_centre.y - cam.y * ts.y, spr->width, spr->height};
					SDL_RenderCopy(eng.get_renderer(), spr->tex, NULL, &dst);
				}
			}
		}
	}
}
