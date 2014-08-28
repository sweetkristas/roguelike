#include <map>

#include "SDL_image.h"

#include "asserts.hpp"
#include "profile_timer.hpp"
#include "texture.hpp"

namespace graphics
{
	namespace
	{
		std::map<std::string, texture>& texture_cache()
		{
			static std::map<std::string, texture> res;
			return res;
		}
	}

	texture::texture(SDL_Renderer* r)
		: renderer_(r)
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
		SDL_SetSurfaceBlendMode(source, SDL_BLENDMODE_NONE);
		// If the source area is empty then default to the whole image.
		if(tex->area_.empty()) {
			tex->area_ = rect(0, 0, source->w, source->h);
		}
		tex->tex_.reset(SDL_CreateTextureFromSurface(tex->renderer_, source), [](SDL_Texture* t) {
			SDL_DestroyTexture(t);
		});
	}

	texture::texture(SDL_Renderer* r, const std::string& fname, TextureFlags flags, const rect& area)
		: area_(area), 
		  flags_(flags)
	{
		load_file_into_texture(fname, this);
	}

	texture::texture(SDL_Renderer* r, const surface& surf, TextureFlags flags, const rect& area)
		: area_(area), 
		  flags_(flags)
	{
		texture_from_surface(const_cast<SDL_Surface*>(surf.get()), this);
	}

	void texture::rebuild_cache()
	{
		for(auto it = texture_cache().begin(); it != texture_cache().end(); ++it) {
			load_file_into_texture(it->first, &it->second);
		}
	}

	void texture::update(const std::string& fname, const rect& area)
	{
		set_area(area);
		load_file_into_texture(fname, this);
	}

	void texture::update(const surface& surf, const rect& area)
	{
		set_area(area);
		texture_from_surface(const_cast<SDL_Surface*>(surf.get()), this);
	}

	void texture::blit(const rect& dest)
	{
		SDL_Rect src = {area_.x(), area_.y(), area_.w(), area_.h()};
		SDL_Rect dst = {dest.x(), dest.y(), dest.w(), dest.h()};
		SDL_RenderCopy(renderer_, tex_.get(), &src, &dst);
	}

	void texture::set_area(const rect& area)
	{
		area_ = area;
	}
}
