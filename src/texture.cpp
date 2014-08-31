#include <map>

#include "SDL_image.h"

#include "asserts.hpp"
#include "profile_timer.hpp"
#include "texture.hpp"

namespace graphics
{
	namespace
	{
		typedef std::map<std::string, std::shared_ptr<SDL_Texture>> texture_cache_type;

		texture_cache_type& texture_cache()
		{
			static texture_cache_type res;
			return res;
		}
		
		SDL_Renderer*& get_renderer()
		{
			static SDL_Renderer* res = nullptr;
			return res;
		}
	}

	texture::manager::manager(SDL_Renderer* renderer)
	{
		get_renderer() = renderer;
	}

	texture::manager::~manager()
	{
	}

	texture::texture()
	{
	}

	void texture::load_file_into_texture(const std::string& fname, texture* tex)
	{
		SDL_Surface* source = IMG_Load(fname.c_str());
		ASSERT_LOG(source != NULL, "Failed to load image: " << fname << " : " << IMG_GetError());
		texture_from_surface(source, tex);
	}

	void texture::texture_from_surface(SDL_Surface* source, texture* tex)
	{
		ASSERT_LOG(get_renderer() != nullptr, "Renderer not set. call graphics::texture::manager texman(...);");
		SDL_SetSurfaceBlendMode(source, SDL_BLENDMODE_BLEND);
		//SDL_SetSurfaceBlendMode(source, SDL_BLENDMODE_NONE);
		// If the source area is empty then default to the whole image.
		if(tex->area_.empty()) {
			tex->area_ = rect(0, 0, source->w, source->h);
		}
		
		auto ntex = SDL_CreateTextureFromSurface(get_renderer(), source);
		ASSERT_LOG(ntex != nullptr, "Couldn't create texture: " << SDL_GetError());
		tex->tex_.reset(ntex, [](SDL_Texture* t) {
			SDL_DestroyTexture(t);
		});
		SDL_UpdateTexture(ntex, NULL, source->pixels, source->pitch);
	}

	texture::texture(const std::string& fname, TextureFlags flags, const rect& area)
		: area_(area), 
		  flags_(flags)
	{
		if(flags & TextureFlags::NO_CACHE) {
			load_file_into_texture(fname, this);
			return;
		}
		auto it = texture_cache().find(fname);
		if(it == texture_cache().end()) {
			load_file_into_texture(fname, this);
			texture_cache()[fname] = tex_;
		} else {
			tex_ = it->second;
		}
	}

	texture::texture(int w, int h, TextureFlags flags)
		: flags_(flags)
	{
		ASSERT_LOG(get_renderer() != nullptr, "Renderer not set. call graphics::texture::manager texman(...);");
		auto ntex = SDL_CreateTexture(get_renderer(), 
			SDL_PIXELFORMAT_ABGR8888, 
			flags_ & TextureFlags::TARGET ? SDL_TEXTUREACCESS_TARGET : SDL_TEXTUREACCESS_STATIC, 
			w, 
			h);
		ASSERT_LOG(ntex != nullptr, "Couldn't create texture: " << SDL_GetError());
		tex_.reset(ntex, [](SDL_Texture* t) {
			SDL_DestroyTexture(t);
		});
	}

	texture::texture(const surface_ptr& surf, TextureFlags flags, const rect& area)
		: area_(area), 
		  flags_(flags)
	{
		texture_from_surface(const_cast<SDL_Surface*>(surf->get()), this);
	}

	void texture::rebuild_cache()
	{
		ASSERT_LOG(get_renderer() != nullptr, "Renderer not set. call graphics::texture::manager texman(...);");
		for(auto it = texture_cache().begin(); it != texture_cache().end(); ++it) {
			SDL_Surface* source = IMG_Load(it->first.c_str());
			ASSERT_LOG(source != NULL, "Failed to load image: " << it->first << " : " << IMG_GetError());
			SDL_SetSurfaceBlendMode(source, SDL_BLENDMODE_BLEND);
			it->second.reset(SDL_CreateTextureFromSurface(get_renderer(), source), [](SDL_Texture* t) {
				SDL_DestroyTexture(t);
			});
		}
	}

	void texture::update(const std::string& fname, const rect& area)
	{
		set_area(area);
		load_file_into_texture(fname, this);
		if(!(flags_ & TextureFlags::NO_CACHE)) {
			texture_cache()[fname] = tex_;
		}
	}

	void texture::update(const surface_ptr& surf, const rect& area)
	{
		set_area(area);
		texture_from_surface(const_cast<SDL_Surface*>(surf->get()), this);
	}

	void texture::blit(const rect& dest)
	{
		ASSERT_LOG(get_renderer() != nullptr, "Renderer not set. call graphics::texture::manager texman(...);");
		SDL_Rect src = {area_.x(), area_.y(), area_.w(), area_.h()};
		SDL_Rect dst = {dest.x(), dest.y(), dest.w() == 0 ? area_.w() : dest.w(), dest.h() == 0 ? area_.h() : dest.h()};
		SDL_RenderCopy(get_renderer(), tex_.get(), &src, &dst);
	}

	void texture::set_area(const rect& area)
	{
		area_ = area;
	}
}
