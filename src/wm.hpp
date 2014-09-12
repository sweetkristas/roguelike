#pragma once

#include "geometry.hpp"

namespace graphics
{
	class window_manager
	{
	public:
		window_manager();
		void create_window(const std::string& title, int x, int y, int w, int h, Uint32 flags);
		void gl_init();
		void set_icon(const std::string& icon);
		SDL_Window* get_window() { return window_; }
		SDL_Renderer* get_renderer() { return renderer_; }
		void swap();
		virtual ~window_manager();	
		int width() const { return width_; }
		int height() const { return height_; }
		const rect& get_viewport() const { return area_; }
		const point get_dim() const { return area_.dimensions(); }
		void update_window_size();
		static window_manager& get_main_window();
	private:
		SDL_Window* window_;
		SDL_GLContext glcontext_;
		SDL_Renderer* renderer_;
		
		// area_ is a synonym for (0,0,width_,height_)
		// we maintain it to give quick access to the screen
		// area rather than creating a temporary rect everytime.
		int width_;
		int height_;
		rect area_;
	};
}
