#include "button.hpp"
#include "gui_elements.hpp"

namespace gui
{
	button::button(const rect& pos, std::function<void()> pressed, widget_ptr child)
		: pressed_fn_(pressed),
		  child_(child),
		  is_pressed_(false)
	{
		normal_tex_ = gui::section::get("buttonLong_brown");
		pressed_tex_ = gui::section::get("buttonLong_brown_pressed");
		//mouse_over_tex_ = gui::section::get("");

		if(pos.empty()) {
			set_area(rect(pos.top_left(), pos.top_left() + point(pressed_tex_.width(), pressed_tex_.height())));
		} else {
			set_area(pos);
		}
	}

	void button::handle_draw(const point& p, float rotation, float scale) const
	{
		const auto& tex = is_pressed_ ? pressed_tex_ : normal_tex_;
		rect area = get_area() * scale;
		tex.blit_ex(rect(p, area.bottom_right()), rotation, area.mid(), graphics::FlipFlags::NONE);

		if(child_) {
			child_->draw(p, rotation, scale);
		}
	}

	bool button::handle_events(SDL_Event* evt, bool claimed)
	{
		if(child_ && child_->process_events(evt, claimed)) {
			return true;
		}
		switch(evt->type) {
			case SDL_MOUSEMOTION: break;
			case SDL_MOUSEBUTTONDOWN:
				if(in_widget(point(evt->button.x, evt->button.y))) {
					// XXX Change texture to pressed one
					on_press();
					return true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				on_release();
				break;
			default:
				break;
		}
		return false;
	}

	void button::on_press()
	{
		is_pressed_ = true;
		if(pressed_fn_) {
			pressed_fn_();
		}
		set_area(rect(get_area().top_left(), get_area().top_left() + point(pressed_tex_.width(), pressed_tex_.height())));
	}

	void button::on_release()
	{
		if(is_pressed_) {
			// XXX Change texture to normal one
			is_pressed_ = false;
		}
		set_area(rect(get_area().top_left(), get_area().top_left() + point(normal_tex_.width(), normal_tex_.height())));
	}
}
