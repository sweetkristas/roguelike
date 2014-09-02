#pragma once

#include "engine_fwd.hpp"
#include "geometry.hpp"
#include "particles.hpp"
#include "process.hpp"
#include "profile_timer.hpp"
#include "quadtree.hpp"
#include "wm.hpp"

enum class EngineState {
	PLAY,
	PAUSE,
	QUIT,
};

enum class EngineUserEvents {
	NEW_TURN = 1,
};

class engine
{
public:
	engine(graphics::window_manager& wm);
	~engine();
	
	void add_entity(component_set_ptr e);
	void remove_entity(component_set_ptr e);

	void add_process(process::process_ptr s);
	void remove_process(process::process_ptr s);

	graphics::window_manager& get_window() { return wm_; }
	const graphics::window_manager& get_window() const { return wm_; }

	SDL_Renderer* get_renderer() { return wm_.get_renderer(); }
	const SDL_Renderer* get_renderer() const { return wm_.get_renderer(); }

	void set_state(EngineState state) { state_ = state; }
	EngineState get_state() const { return state_; }

	bool update(double time);

	int get_turns() const { return turns_; }
	void inc_turns(int cnt = 1);

	void set_camera(const point& cam) { camera_ = cam; }
	const point& get_camera() { return camera_; }

	particle::particle_system_manager& get_particles() { return particles_; }

	entity_list entities_in_area(const rect& r);

	const point& get_tile_size() const { return tile_size_; }
	void set_tile_size(const point& p) { tile_size_ = p; }

private:
	void translate_mouse_coords(SDL_Event* evt);
	void process_events();
	void populate_quadtree();
	EngineState state_;
	int turns_;
	point camera_;
	graphics::window_manager& wm_;
	entity_list entity_list_;
	quadtree<component_set_ptr> entity_quads_;
	std::vector<process::process_ptr> process_list_;
	point tile_size_;
	particle::particle_system_manager particles_;
};
