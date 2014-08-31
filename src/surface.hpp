#pragma once

#include <memory>
#include "SDL.h"

#include "geometry.hpp"

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#	define SURFACE_MASK 0xFF,0xFF00,0xFF0000,0xFF000000
#	define SURFACE_MASK_RGB 0xFF,0xFF00,0xFF0000,0x0
#	define SURFACE_MASK_R 0xff
#	define SURFACE_MASK_G 0xff00
#	define SURFACE_MASK_B 0xff0000
#	define SURFACE_MASK_A 0xff000000
#else
#	define SURFACE_MASK 0xFF000000,0xFF0000,0xFF00,0xFF
#	define SURFACE_MASK_RGB 0xFF0000,0xFF00,0xFF,0x0
#	define SURFACE_MASK_R 0xff000000
#	define SURFACE_MASK_G 0xff0000
#	define SURFACE_MASK_B 0xff00
#	define SURFACE_MASK_A 0xff
#endif

namespace graphics
{
	class surface;
}
typedef std::shared_ptr<graphics::surface> surface_ptr;

namespace graphics
{
	class surface
	{
	public:
		explicit surface(int width, int height);
		explicit surface(const std::string& fname);
		explicit surface(SDL_Surface* surf);
		~surface();

		SDL_Surface* get() { return surf_.get(); }
		const SDL_Surface* get() const { return surf_.get(); }

		int width() const { return surf_->w; }
		int height() const { return surf_->h; }

		// clipped blit
		void blit_clipped(const surface_ptr& src, const rect& dst_rect=rect());
		// scaled blit
		void blit_scaled(const surface_ptr& src, const rect& dst_rect);
		void blit_scaled(const surface_ptr& src, const rect& src_rect, const rect& dst_rect);
	private:
		surface();
		std::shared_ptr<SDL_Surface> surf_;
	};
}
