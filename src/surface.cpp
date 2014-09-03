#include "SDL_image.h"

#include "asserts.hpp"
#include "surface.hpp"

namespace graphics
{
	namespace
	{
		typedef std::map<std::string, surface_ptr> surface_file_cache;
		surface_file_cache& get_file_cache() 
		{
			static surface_file_cache res;
			return res;
		}
	}

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

	void surface::blit_clipped(const surface_ptr& src, const rect& dst_rect)
	{
		if(dst_rect.empty()) {
			SDL_BlitSurface(src->surf_.get(), NULL, surf_.get(), NULL);
		} else {
			SDL_Rect dr = {dst_rect.x(), dst_rect.y(), dst_rect.w(), dst_rect.h()};
			SDL_BlitSurface(src->surf_.get(), NULL, surf_.get(), &dr);
		}
	}

	void surface::blit_scaled(const surface_ptr& src, const rect& dst_rect)
	{
		SDL_Rect dr = {dst_rect.x(), dst_rect.y(), dst_rect.w(), dst_rect.h()};
		SDL_BlitSurface(src->surf_.get(), NULL, surf_.get(), &dr);
	}

	void surface::blit_scaled(const surface_ptr& src, const rect& src_rect, const rect& dst_rect)
	{
		SDL_Rect sr = {src_rect.x(), src_rect.y(), src_rect.w(), src_rect.h()};
		SDL_Rect dr = {dst_rect.x(), dst_rect.y(), dst_rect.w(), dst_rect.h()};
		SDL_BlitSurface(src->surf_.get(), &sr, surf_.get(), &dr);
	}

	void surface::save(const std::string& filename)
	{
		int res = IMG_SavePNG(surf_.get(), filename.c_str());
		ASSERT_LOG(res == 0, "Error save image file: " << IMG_GetError());
	}

	surface_ptr surface::create(int width, int height)
	{
		return std::make_shared<graphics::surface>(width, height);
	}

	surface_ptr surface::create(const std::string& fname)
	{
		auto it = get_file_cache().find(fname);
		if(it == get_file_cache().end()) {
			auto surf = std::make_shared<graphics::surface>(fname);
			get_file_cache()[fname] = surf;
			return surf;
		}
		return it->second;
	}

	surface_ptr surface::create(SDL_Surface* surf)
	{
		return std::make_shared<graphics::surface>(surf);
	}
}
