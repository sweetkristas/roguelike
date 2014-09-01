#pragma once

#include "node.hpp"
#include "texture.hpp"

namespace gui
{
	namespace section
	{
		struct manager
		{
			manager(SDL_Renderer* renderer, const node& n);
			~manager();
		};

		graphics::texture get(const std::string& name);
	}
}
