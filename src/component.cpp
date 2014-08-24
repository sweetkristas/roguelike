#include "asserts.hpp"
#include "component.hpp"

namespace component
{
	sprite::sprite(SDL_Renderer* renderer, SDL_Surface* surf) 
		: component(Component::SPRITE),
		  tex(NULL)
	{
		if(surf != NULL) {
			tex = SDL_CreateTextureFromSurface(renderer, surf);
			ASSERT_LOG(tex != NULL, "Couldn't create texture from surface: " << SDL_GetError());
			width = surf->w;
			height = surf->h;
			SDL_FreeSurface(surf);
		}
	}

	sprite::~sprite()
	{
		if(tex != NULL) {
			SDL_DestroyTexture(tex);
		}
	}

	void sprite::update_texture(SDL_Renderer* renderer, SDL_Surface* surf)
	{
		if(tex != NULL) {
			SDL_DestroyTexture(tex);
		}
		if(surf != NULL) {
			tex = SDL_CreateTextureFromSurface(renderer, surf);
			ASSERT_LOG(tex != NULL, "Couldn't create texture from surface: " << SDL_GetError());
			width = surf->w;
			height = surf->h;
			SDL_FreeSurface(surf);
		}
	}

	lights::lights() 
		: component(Component::LIGHTS), 
		  tex(NULL)
	{
	}

	lights::~lights()
	{
		if(tex != NULL) {
			SDL_DestroyTexture(tex);
		}
	}

	mapgrid::mapgrid(const node& n)
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
	}
}
