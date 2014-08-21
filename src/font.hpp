#pragma once

#include <memory>
#include "graphics.hpp"

namespace font
{
	class manager
	{
	public:
		manager();
		~manager();
	};

	typedef std::shared_ptr<TTF_Font> font_ptr;
	font_ptr get_font(const std::string& font_name, int size);
}
