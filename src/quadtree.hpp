#pragma once

#include <set>
#include <vector>
#include "geometry.hpp"

template<typename T, typename R=int, unsigned MAX_OBJECTS=10, unsigned MAX_LEVELS=5>
class quadtree
{
public:
	explicit quadtree(int level, const geometry::Rect<R>& bounds) 
		: level_(level), 
		  bounding_rect_(bounds) 
	{
		nodes_.reserve(4);
	}
	void clear() {
		objects_.clear();
		for(auto& n : nodes_) {
			n.clear();
		}
		nodes_.clear();
	}
	void split() {
		int sw = bounding_rect_.w() >> 1;
		int sh = bounding_rect_.h() >> 1;

		nodes_.emplace_back(level_+1, geometry::Rect<R>(x+sw, y,    sw, sh));
		nodes_.emplace_back(level_+1, geometry::Rect<R>(x,    y,    sw, sh));
		nodes_.emplace_back(level_+1, geometry::Rect<R>(x+sw, y+sh, sw, sh));
		nodes_.emplace_back(level_+1, geometry::Rect<R>(x+sw, y+sh, sw, sh));
	}
	void insert(const T& obj, const geometry::Rect<R>& r) {
		if(!nodes_.empty()) {
			int index = get_index(r);
			if(index != -1) {
				nodes[index].insert(obj, r);
				return;
			}
		}
		objects_.insert(std::make_pair(obj, r));
		if(objects_.size() > MAX_OBJECTS && level_ < MAX_LEVELS) {
			if(nodes_.empty()) {
				split();
			}
			auto it = objects_.begin();
			while(it != objects_.end()) {
				int index = get_index(it->second);
				if(index != -1) {
					nodes[index].insert(it->first, it->second);
					objects_.erase(it++);
				} else {
					++it;
				}
			}
		}
	}
private:
	int level_;
	geometry::Rect<R> bounding_rect_;
	std::set<std::pair<T,geometry::Rect<R>>> objects_;
	std::vector<quadtree<T, R, MAX_OBJECTS, MAX_LEVELS>> nodes_;

	int get_index(R r) {
		int index = -1;
		bool tq = r.y() < bounding_rect_.mid_y() && r.y2() < bounding_rect_.mid_y();
		bool bq = r.y() > bounding_rect_.mid_y();
		if(r.x() < bounding_rect_.mid_x() && r.x2() < bounding_rect_.mid_x()) {
			if(tq) {
				index = 1;
			} else if(bq) {
				index = 2;
			}
		} else if(r.x() > bounding_rect_.mid_x()) {
			if(tq) {
				index = 0;
			} else if(bq) {
				index = 3;
			}
		}
		return index;
	}
};
