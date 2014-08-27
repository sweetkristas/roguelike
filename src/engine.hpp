#pragma once

#include <bitset>
#include <vector>

#include "component.hpp"
#include "entity.hpp"
#include "geometry.hpp"
#include "particles.hpp"
#include "process.hpp"
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
	void inc_turns(int cnt = 1);

	void set_camera(const std::shared_ptr<component::position>& cam) { camera_ = cam; }
	const point& get_camera();

	particle::particle_system_manager& get_particles() { return particles_; }

	std::vector<entity_ptr> entities_in_area(const rect& r);

	const point& get_tile_size() const { return tile_size_; }
	void set_tile_size(const point& p) { tile_size_ = p; }

private:
	void process_events();
	void populate_quadtree();
	EngineState state_;
	int turns_;
	std::shared_ptr<component::position> camera_;
	graphics::window_manager& wm_;
	std::vector<entity_ptr> entity_list_;
	quadtree<entity_ptr> entity_quads_;
	std::vector<process::process_ptr> process_list_;
	point tile_size_;
	particle::particle_system_manager particles_;
};
