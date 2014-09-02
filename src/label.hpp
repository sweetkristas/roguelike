#pragma once

#include "color.hpp"
#include "font.hpp"
#include "widget.hpp"

namespace gui
{
	class label : public widget
	{
	public:
		label(const rectf& pos, Justify justify, const std::string& utf8, const graphics::color& color=graphics::color(), int sz=12);

		void set_text(const std::string& utf8);
		void set_size(int sz);
		void set_color(const graphics::color& color);
		void set_font(const std::string& font_name);
	private:
		void handle_draw(const rect& r, float rotation, float scale) const override;
		void recalc_dimensions() override;
		void init();
		std::string text_;
		int size_;
		graphics::color color_;
		std::string font_name_;
		font::font_ptr font_;
		graphics::texture label_tex_;
	};
}
