#include "button.hpp"
#include "gui_elements.hpp"

namespace gui
{
	button::button(const rect& pos, std::function<void()> pressed, widget_ptr child)
		: pressed_fn_(pressed),
		  child_(child),
		  is_pressed_(false)
	{
		set_area(pos);
		normal_tex_ = gui::section::get("buttonLong_brown");
		pressed_tex_ = gui::section::get("buttonLong_brown_pressed");
		mouse_over_tex_ = gui::section::get("");
	}

	void button::handle_draw(const point& p, float rotation, float scale) const
	{
		// XXX Draw button at location p using appropriate rotation and scale.		

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
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
				if(in_widget(point(evt->button.x, evt->button.y))) {
					// XXX Change texture to pressed one
					is_pressed_ = true;
					if(pressed_fn_) {
						pressed_fn_();
					}
					return true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(is_pressed_) {
					// XXX Change texture to normal one
					is_pressed_ = false;
				}
				break;
			default:
				break;
		}
		return false;
	}
}
