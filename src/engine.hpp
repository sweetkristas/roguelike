#pragma once

#include <bitset>
#include <vector>

#include "component.hpp"
#include "entity.hpp"
#include "geometry.hpp"
#include "process.hpp"
#include "quadtree.hpp"
#include "wm.hpp"

enum class EngineState {
	PLAY,
	PAUSE,
	QUIT,
};

class engine
{
public:
	engine(graphics::window_manager& wm);
	~engine();
	
	void add_entity(entity_ptr e);
	void remove_entity(entity_ptr e);

	void add_process(process::process_ptr s);
	void remove_process(process::process_ptr s);

	graphics::window_manager& get_window() { return wm_; }

	SDL_Renderer* get_renderer() { return wm_.get_renderer(); }

	void set_state(EngineState state) { state_ = state; }
	EngineState get_state() const { return state_; }

	bool update(double time);

	int get_turns() const { return turns_; }
	void inc_turns() { ++turns_; }

	void set_camera(const point& cam) { camera_ = cam; }
	const point& get_camera() { return camera_; }

	std::vector<entity_ptr> entities_in_area(const rect& r);
private:
	void process_events();
	void populate_quadtree();
	EngineState state_;
	int turns_;
	point camera_;
	graphics::window_manager& wm_;
	std::vector<entity_ptr> entity_list_;
	quadtree<entity_ptr> entity_quads_;
	std::vector<process::process_ptr> process_list_;
};
