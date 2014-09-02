#include "widget.hpp"
#include "wm.hpp"

namespace gui
{
	widget::widget(const rectf& r, Justify justify)
		: zorder_(0), 
		  rotation_(0), 
		  scale_(1.0f),
		  area_set_(false),
		  just_(justify),
		  parent_(nullptr)
	{
		if(r.empty()) {
			set_loc_internal(r.top_left());
		} else {
			set_area(r);
		}
	}

	widget::~widget()
	{
	}

	bool widget::in_widget(const point& p)
	{
		pointf pf(static_cast<float>(p.x/get_mouse_scale_factor()), static_cast<float>(p.y/get_mouse_scale_factor()));
		return geometry::pointInRect(pf, real_area_ * get_scale());
	}

	void widget::draw(const rect& r, float rotation, float scale) const
	{
		ASSERT_LOG(!real_area_.empty(), "No dimensions set.");
		rect adjust(r.x() + static_cast<int>(r.w() * real_area_.x()),
			r.y() + static_cast<int>(r.h() * real_area_.y()),
			static_cast<int>(r.w() * real_area_.w()),
			static_cast<int>(r.h() * real_area_.h()));
		handle_draw(adjust, rotation+rotation_, scale*scale_);
	}

	bool widget::process_events(SDL_Event* evt, bool claimed)
	{
		if(claimed) {
			return claimed;
		}
		return handle_events(evt, claimed);
	}

	void widget::update(engine& eng, double t)
	{
		handle_update(eng, t);
	}

	void widget::set_dim_internal(int w, int h) 
	{ 
		// XXXXX BUG this shoud be relative to the parent item no the main window
		float pw = w / get_parent_absolute_width();
		float ph = h / get_parent_absolute_height();
		if(parent_) {
			pw /= parent_->area_.w();
			ph /= parent_->area_.h();
		}
		area_ = rectf(area_.top_left(), area_.top_left() + pointf(pw, ph));
		update_area();
	}

	void widget::set_justification(Justify j)
	{
		just_ = j;
		update_area();
	}

	void widget::set_loc_internal(const pointf& loc) 
	{ 
		area_ = rectf(loc, area_.bottom_right()); 
		update_area();
	}

	void widget::update_area()
	{
		float x = area_.x();
		float y = area_.y();
		float w = area_.w();
		float h = area_.h();
		if(just_ & Justify::H_CENTER) {
			x = 0.5f - w/2 + x;
		} else if(just_ & Justify::RIGHT) {
			x = 1.0f - w + x;
		}
		if(just_ & Justify::V_CENTER) {
			y = 0.5f - h/2 + y;
		} else if(just_ & Justify::BOTTOM) {
			y = 1.0f - h + y;
		}
		real_area_ = rectf(x, y, w, h);
	}

	void widget::set_parent(widget* parent)
	{
		parent_ = parent;
		recalc_dimensions();
	}

	float widget::get_parent_absolute_width()
	{
		auto& wm = graphics::window_manager::get_main_window();
		if(parent_ == nullptr) {
			return static_cast<float>(wm.width());
		}
		return parent_->get_parent_absolute_width();
	}

	float widget::get_parent_absolute_height()
	{
		auto& wm = graphics::window_manager::get_main_window();
		if(parent_ == nullptr) {
			return static_cast<float>(wm.height());
		}
		return parent_->get_parent_absolute_height();
	}
}
