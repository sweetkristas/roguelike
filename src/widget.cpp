#include "widget.hpp"

namespace gui
{
	widget::widget()
		: zorder_(0)
	{
	}

	widget::~widget()
	{
	}

	bool widget::in_widget(const point& p)
	{
		return geometry::pointInRect(p, area_);
	}

	void widget::draw(const point& p, float rotation, float scale) const
	{
		handle_draw(p + area_.top_left(), rotation+rotation_, scale+scale_);
	}

	bool widget::process_events(SDL_Event* evt, bool claimed)
	{
		if(claimed) {
			return claimed;
		}
		return handle_events(evt, claimed);
	}

	void widget::update(double t)
	{
		handle_update(t);
	}
}