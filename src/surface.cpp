#include "SDL_image.h"

#include "asserts.hpp"
#include "surface.hpp"

namespace graphics
{
	surface::surface(int width, int height)
	{
		// Generates an empty surface of size (width,height)
		// defaults to a 32-bit ABGR surface
		auto surf = SDL_CreateRGBSurface(0, width, height, 32, SURFACE_MASK);
		ASSERT_LOG(surf != NULL, "Surface was null.");
		surf_.reset(surf, [](SDL_Surface* surf) {
			SDL_FreeSurface(surf);
		});
	}

	surface::surface(const std::string& fname)
	{
		auto surf = IMG_Load(fname.c_str());
		ASSERT_LOG(surf != NULL, "Failed to load image: " << fname << " : " << IMG_GetError());
		surf_.reset(surf, [](SDL_Surface* surf){
			SDL_FreeSurface(surf);
		});
	}

	surface::surface(SDL_Surface* surf)
	{
		ASSERT_LOG(surf != NULL, "Surface passed in was null.");
		surf_.reset(surf, [](SDL_Surface* surf) {
			SDL_FreeSurface(surf);
		});
	}

	surface::~surface()
	{
	}

	void surface::blit_clipped(const surface& src, const rect& dst_rect)
	{
		SDL_Rect dr = {dst_rect.x(), dst_rect.y(), dst_rect.w(), dst_rect.h()};
		SDL_BlitSurface(src.surf_.get(), NULL, surf_.get(), &dr);
	}

	void surface::blit_scaled(const surface& src, const rect& dst_rect)
	{
		SDL_Rect dr = {dst_rect.x(), dst_rect.y(), dst_rect.w(), dst_rect.h()};
		SDL_BlitSurface(src.surf_.get(), NULL, surf_.get(), &dr);
	}
}
