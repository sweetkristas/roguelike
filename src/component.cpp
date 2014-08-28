#include "asserts.hpp"
#include "component.hpp"

namespace component
{
	Component get_component_from_string(const std::string& s)
	{
		if(s == "position") {
			return Component::POSITION;
		} else if(s == "sprite") {
			return Component::SPRITE;
		} else if(s == "stats") {
			return Component::STATS;
		} else if(s == "ai") {
			return Component::AI;
		} else if(s == "input") {
			return Component::INPUT;
		} else if(s == "lights") {
			return Component::LIGHTS;
		} else if(s == "map") {
			return Component::MAP;
		} else if(s == "player") {
			return Component::PLAYER;
		} else if(s == "enemy") {
			return Component::ENEMY;
		} else if(s == "gui") {
			return Component::GUI;
		} else if(s == "collision") {
			return Component::COLLISION;
		}
		ASSERT_LOG(false, "Unrecognised component string '" << s << "'");
		return static_cast<Component>(0);
	}

	sprite::sprite(const std::string& filename, const rect& area)
		: component(Component::SPRITE),
		  tex(filename, graphics::TextureFlags::NONE, area)
	{
	}

	sprite::sprite(surface_ptr surf, const rect& area) 
		: component(Component::SPRITE),
		  tex(surf, graphics::TextureFlags::NONE, area)
	{
	}

	sprite::~sprite()
	{
	}

	void sprite::update_texture(surface_ptr surf)
	{
		tex.update(surf);
	}

	lights::lights() 
		: component(Component::LIGHTS)
	{
	}

	lights::~lights()
	{
	}

	mapgrid::mapgrid()
		: component(Component::MAP)
	{
	}

	/*mapgrid::mapgrid(const node& n)
		: component(Component::MAP)
	{
		auto mp = n["map"].as_list_strings();
		map.reserve(mp.size());
		int y = 0;
		for(auto& row : mp) {
			int x = 0;
			std::vector<MapSymbols> new_row;
			new_row.reserve(row.size());
			for(auto& col : row) {
				MapSymbols sym = convert_map_symbol(col);
				if(sym == MapSymbols::EXIT) {
					exits.emplace_back(x, y);
					sym = MapSymbols::DIRT;
				} else if(sym == MapSymbols::START) {
					start = point(x, y);
					sym = MapSymbols::DIRT;
				}
				new_row.emplace_back(sym);
				++x;
			}
			map.emplace_back(new_row);
			++y;
		}
	}*/
}
