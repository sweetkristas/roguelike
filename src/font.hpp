#pragma once

#include <memory>

#include "SDL.h"
#include "SDL_ttf.h"

#include "color.hpp"
#include "surface.hpp"

namespace font
{
	class manager
	{
	public:
		manager();
		~manager();
	};

	struct glyph_metrics 
	{
		int minx;
		int maxx;
		int miny;
		int maxy;
		int advance;
	};

	typedef std::shared_ptr<TTF_Font> font_ptr;
	font_ptr get_font(const std::string& font_name, int size);
	void get_glyph_metrics(const font_ptr& fnt, int ch, int *w, int *h, glyph_metrics* gm=NULL);
	void get_text_size(const font_ptr& fnt, const std::string& utf8, int* w, int* h);
	SDL_Surface* render(const std::string& utf8, const font_ptr& fnt, const graphics::color& color);
	SDL_Surface* render_shaded(const std::string& utf8, const font_ptr& fnt, const graphics::color& fg, const graphics::color& bg);
}
