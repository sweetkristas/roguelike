#include "button.hpp"
#include "gui_elements.hpp"
#include "wm.hpp"

namespace gui
{
	button::button(const rectf& r, Justify justify, std::function<void()> pressed, widget_ptr child)
		: widget(r, justify),
		  pressed_fn_(pressed),
		  child_(child),
		  is_pressed_(false),
		  is_mouseover_(false)
	{
		ASSERT_LOG(r.x() >= -1.0f && r.x() <= 1.0f, "Button X position exceeds parent boundaries.");
		ASSERT_LOG(r.y() >= -1.0f && r.y() <= 1.0f, "Button Y position exceeds parent boundaries.");
		ASSERT_LOG(r.x2() >= -1.0f && r.x2() <= 1.0f, "Button X2 position exceeds parent boundaries.");
		ASSERT_LOG(r.y2() >= -1.0f && r.y2() <= 1.0f, "Button Y2 position exceeds parent boundaries.");

		normal_tex_ = gui::section::get("buttonLong_brown");
		pressed_tex_ = gui::section::get("buttonLong_brown_pressed");
		mouse_over_tex_ = gui::section::get("mouseover_overlay");
		mouse_over_tex_.set_blend(graphics::BlendMode::ADDITIVE);
		normal_tex_.set_blend(graphics::BlendMode::BLEND);
		pressed_tex_.set_blend(graphics::BlendMode::BLEND);

		if(!is_area_set()) {
			set_dim_internal(normal_tex_.width(), normal_tex_.height());
		}
		if(child_) {
			child_->set_parent(this);
		}
	}

	void button::handle_draw(const rect& r, float rotation, float scale) const
	{
		const auto& tex = is_pressed_ ? pressed_tex_ : normal_tex_;
		tex.blit_ex(r * scale, rotation, r.mid() * scale, graphics::FlipFlags::NONE);
		if(is_mouseover_) {
			mouse_over_tex_.blit_ex(r * scale, rotation, r.mid() * scale, graphics::FlipFlags::NONE);
		}

		if(child_) {
			child_->draw(r, rotation, scale);
		}
	}

	bool button::handle_events(SDL_Event* evt, bool claimed)
	{
		if(child_ && child_->process_events(evt, claimed)) {
			return true;
		}
		switch(evt->type) {
			case SDL_MOUSEMOTION: 
				if(in_widget(point(evt->button.x, evt->button.y))) {
					is_mouseover_ = true;
				} else {
					is_mouseover_ = false;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(in_widget(point(evt->button.x, evt->button.y))) {
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
		if(!is_area_set()) {
			set_dim_internal(pressed_tex_.width(), pressed_tex_.height());
		}
	}

	void button::on_release()
	{
		if(is_pressed_) {
			// XXX Change texture to normal one
			is_pressed_ = false;
		}
		if(!is_area_set()) {
			set_dim_internal(normal_tex_.width(), normal_tex_.height());
		}
	}

	void button::recalc_dimensions()
	{
		if(!is_area_set()) {
			if(is_pressed_) {
				set_dim_internal(pressed_tex_.width(), pressed_tex_.height());
			} else {
				set_dim_internal(normal_tex_.width(), normal_tex_.height());
			}
		}
	}
}
