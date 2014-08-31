#pragma once

#include "geometry.hpp"
#include "surface.hpp"

namespace graphics
{
	enum class TextureFlags
	{
		NONE		= 0,
		NO_CACHE	= 1,
		TARGET		= 2,
	};
	
	inline TextureFlags operator|(TextureFlags lhs, TextureFlags rhs)
	{
		return static_cast<TextureFlags>(static_cast<int>(lhs) | static_cast<int>(rhs));
	}

	inline bool operator&(TextureFlags lhs, TextureFlags rhs)
	{
		return ((static_cast<int>(lhs) & static_cast<int>(rhs)) == static_cast<int>(rhs));
	}

	class texture
	{
	public:
		struct manager
		{
			manager(SDL_Renderer* r);
			~manager();
		};

		texture();
		explicit texture(int w, int h, TextureFlags flags);
		explicit texture(const std::string& fname, TextureFlags flags, const rect& area=rect());
		explicit texture(const surface_ptr& surf, TextureFlags flags, const rect& area=rect());

		const rect& get_area() const { return area_; }
		void set_area(const rect& area);

		void update(const std::string& fname, const rect& area = rect());
		void update(const surface_ptr& surf, const rect& area = rect());

		SDL_Texture* get() { return tex_.get(); }

		bool is_valid() { return tex_ != nullptr; }

		void blit(const rect& dst);
		//void blit_ex(SDL_Renderer* renderer, const rect& dst, double angle, const point& center, FlipFlags flip);
		static void rebuild_cache();
	private:
		static void texture_from_surface(SDL_Surface* source, texture* tex);
		static void load_file_into_texture(const std::string& fname, texture* tex);
		rect area_;
		TextureFlags flags_;
		std::shared_ptr<SDL_Texture> tex_;
	};
}
