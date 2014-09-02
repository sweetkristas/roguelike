#pragma once

#include "widget.hpp"

namespace gui
{
	class dialog : public widget
	{
	public:
		dialog(const rectf& pos, Justify justify);
		void close();
		void add_widget(widget_ptr w);
	private:
		virtual bool handle_events(SDL_Event* evt, bool claimed) override;
		void handle_draw(const rect& r, float rotation, float scale) const override;
		void handle_update(engine& eng, double t) override;
		void recalc_dimensions() override;
		void init();
		graphics::texture bg_;
		std::vector<widget_ptr> children_;
		bool is_open_;
	};
}
