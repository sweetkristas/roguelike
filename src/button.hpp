#pragma once

#include "widget.hpp"

namespace gui
{
	class button : public widget
	{
	public:
		button(const rect& pos, std::function<void()> pressed, widget_ptr child=nullptr);
	private:
		void handle_draw(const point& p, float rotation, float scale) const override;
		bool handle_events(SDL_Event* evt, bool claimed) override;
		void on_press();
		void on_release();
		std::function<void()> pressed_fn_;
		widget_ptr child_;
		bool is_pressed_;
		graphics::texture normal_tex_;
		graphics::texture pressed_tex_;
		graphics::texture mouse_over_tex_;
	};
}
