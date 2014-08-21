#pragma once

namespace graphics
{
	class window_manager
	{
	public:
		window_manager();
		void create_window(const std::string& title, int x, int y, int w, int h, Uint32 flags);
		void gl_init();
		void set_icon(const std::string& icon);
		void swap();
		virtual ~window_manager();	
	private:
		SDL_Window* window_;
		SDL_GLContext glcontext_;
		SDL_Renderer* renderer_;
		int width_;
		int height_;
	};
}