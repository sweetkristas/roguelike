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
		SDL_Renderer* get_renderer() { return renderer_; }
		void swap();
		virtual ~window_manager();	
		int width() const { return width_; }
		int height() const { return height_; }
		void update_window_size();
	private:
		SDL_Window* window_;
		SDL_GLContext glcontext_;
		SDL_Renderer* renderer_;
		int width_;
		int height_;
	};
}