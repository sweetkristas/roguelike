#pragma once

#include "color.hpp"
#include "font.hpp"
#include "geometry.hpp"
#include "shaders.hpp"
#include "texture.hpp"

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
				const color& c);
		}
	}
}
