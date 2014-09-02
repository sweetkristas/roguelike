#include "label.hpp"

namespace gui
{
	label::label(const rectf& pos, Justify justify, const std::string& utf8, const graphics::color& color, int sz)
		: widget(pos, justify), 
		  text_(utf8),
		  size_(sz),
		  color_(color)
	{
		init();
	}

	void label::set_text(const std::string& utf8)
	{
		text_ = utf8;
		init();
	}

	void label::set_size(int sz)
	{
		size_ = sz;
		init();
	}

	void label::set_color(const graphics::color& color)
	{
		color_ = color;
		init();
	}

	void label::set_font(const std::string& font_name)
	{
		font_name_ = font_name;
		init();
	}

	void label::handle_draw(const rect& r, float rotation, float scale) const
	{
		if(label_tex_.is_valid()) {
			label_tex_.blit_ex(r * scale, rotation, r.mid() * scale, graphics::FlipFlags::NONE);
		}
	}

	void label::recalc_dimensions()
	{
		if(label_tex_.is_valid() && !is_area_set()) {
			set_dim_internal(label_tex_.width(), label_tex_.height());
		}
	}

	void label::init()
	{
		font_ = font::get_font(font_name_.empty() ? font::get_default_font_name() : font_name_, size_);
		auto s = std::make_shared<graphics::surface>(font::render(text_, font_, color_));
		label_tex_ = graphics::texture(s, graphics::TextureFlags::NO_CACHE);
		recalc_dimensions();
	}
}
