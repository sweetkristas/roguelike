#include "asserts.hpp"
#include "profile_timer.hpp"
#include "render.hpp"
#include "render_text.hpp"
#include "surface.hpp"

namespace graphics
{
	namespace render
	{
		namespace text
		{
			rect quick_draw(int x, 
				int y,
				const std::string& str, 
				const std::string& font, 
				int size, 
				const color& c)
			{
				SDL_Color bg = {0, 0, 0, 255};
				auto fnt = font::get_font(font, size);
				int w, h;
				TTF_SizeUTF8(fnt.get(), str.c_str(), &w, &h);
				surface_ptr surf = surface_ptr(new surface(TTF_RenderUTF8_Blended(fnt.get(), str.c_str(), c.as_sdl_color())));
				ASSERT_LOG(surf != NULL, "Couldn't render text into texture");
				blit_2d_texture(texture::get(surf), static_cast<float>(x), static_cast<float>(y), static_cast<float>(surf->width()), static_cast<float>(surf->height()));
				return rect(x, y, w, h);
			}
		}
	}
}
