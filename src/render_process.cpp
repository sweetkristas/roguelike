#include "component.hpp"
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

	void render::update(engine& eng, double t, const entity_list& elist)
	{
		using namespace component;
		static component_id sprite_mask = genmask(Component::POSITION)  | genmask(Component::SPRITE);
		static component_id gui_mask = sprite_mask | genmask(Component::GUI);
		static component_id map_mask = genmask(Component::MAP);
		
		const point& cam = eng.get_camera();
		const point screen_centre(eng.get_window().width() / 2, eng.get_window().height() / 2);
		const point& ts = eng.get_tile_size();

		for(auto& e : elist) {
			if((e->mask & gui_mask) == gui_mask) {
				auto& spr = e->spr;
				auto& pos = e->pos;
				if(spr->tex.is_valid()) {
					spr->tex.blit(rect(pos->pos.x, pos->pos.y, ts.x, ts.y));
				}
			}  else if((e->mask & sprite_mask) == sprite_mask) {
				auto& spr = e->spr;
				auto& pos = e->pos;
				if(spr->tex.is_valid()) {
					spr->tex.blit(rect(screen_centre.x - (cam.x - pos->pos.x) * ts.x, screen_centre.y - (cam.y - pos->pos.y) * ts.y, ts.x, ts.y));
				}
			} else if((e->mask & map_mask) == map_mask) {
				auto& map = e->map;

				const int screen_width_in_tiles = (eng.get_window().width() + eng.get_tile_size().x - 1) / eng.get_tile_size().x;
				const int screen_height_in_tiles = (eng.get_window().height() + eng.get_tile_size().y - 1) / eng.get_tile_size().y;
				rect area = rect::from_coordinates(-screen_width_in_tiles / 2 + cam.x, 
					-screen_height_in_tiles / 2 + cam.y,
					screen_width_in_tiles / 2 + cam.x,
					screen_height_in_tiles / 2 + cam.y);

				for(auto& chk : map->t.get_chunks_in_area(area)) {
					chk->draw(eng, cam);
				}
			}
		}
	}
}
