#pragma once

#include "geometry.hpp"
#include "surface.hpp"

namespace graphics
{
	enum class TextureFlags
	{
		NO_CACHE	= 1,
	};
	
	inline TextureFlags operator|(TextureFlags lhs, TextureFlags rhs)
	{
		return static_cast<TextureFlags>(static_cast<int>(lhs) | static_cast<int>(rhs));
	}

	inline TextureFlags operator&(TextureFlags lhs, TextureFlags rhs)
	{
		return static_cast<TextureFlags>(static_cast<int>(lhs)& static_cast<int>(rhs));
	}

	class texture
	{
	public:
		explicit texture(SDL_Renderer* r);
		explicit texture(SDL_Renderer* r, const std::string& fname, TextureFlags flags, const rect& area=rect());
		explicit texture(SDL_Renderer* r, const surface& surf, TextureFlags flags, const rect& area=rect());

		const rect& get_area() const { return area_; }
		void set_area(const rect& area);

		void update(const std::string& fname, const rect& area = rect());
		void update(const surface& surf, const rect& area = rect());

		void blit(const rect& dst);
		//void blit_ex(SDL_Renderer* renderer, const rect& dst, double angle, const point& center, FlipFlags flip);
		static void rebuild_cache();
	protected:
	private:
		texture();
		static void texture_from_surface(SDL_Surface* source, texture* tex);
		static void load_file_into_texture(const std::string& fname, texture* tex);
		SDL_Renderer* renderer_;
		rect area_;
		TextureFlags flags_;
		std::shared_ptr<SDL_Texture> tex_;
	};
}
