#pragma once

#include <memory>
#include <functional>

#include "SDL.h"

#include "geometry.hpp"
#include "texture.hpp"

namespace gui
{
	class widget
	{
	public:
		widget();
		virtual ~widget();
		void draw(const point& p=point(), float rotation=0.0, float scale=0.0) const;
		bool process_events(SDL_Event* evt, bool claimed);
		void update(double t);

		void set_area(const rect& area) { area_ = area; }
		const rect& get_area() const { return area_; }

		int get_zorder() const { return zorder_; }
		void set_zorder(int z) { zorder_ = z; }

		float get_rotation() const { return rotation_; }
		void set_rotation(float rotation) { rotation_ = rotation; }

		float get_scale() const { return scale_; }
		void set_scale(float scale) { scale_ = scale; }

		bool in_widget(const point& p);
	private:
		virtual void handle_update(double t) {}
		virtual void handle_draw(const point& p, float rotation, float scale) const {}
		virtual bool handle_events(SDL_Event* evt, bool claimed) {}
		rect area_;
		int zorder_;
		float rotation_;
		float scale_;
	};

	typedef std::shared_ptr<widget> widget_ptr;

	inline bool operator<(const widget& lhs, const widget& rhs) { return lhs.get_zorder() < rhs.get_zorder(); }
	inline bool operator<(const widget_ptr& lhs, const widget_ptr& rhs) { return *lhs < *rhs; }
}
