#pragma once

#include <memory>

#include "node.hpp"
#include "texture.hpp"

namespace dungeon
{
	void load_rules(const node& n);

	class dungeon_model;
	typedef std::shared_ptr<dungeon_model> dungeon_model_ptr;

	// Holds a single dungeon level.
	class dungeon_model
	{
	public:
		dungeon_model();
		int width() const { return width_; }
		int height() const { return height_; }
		int get_at(int x, int y);
		// Generate a level, populating it with things.
		static dungeon_model_ptr generate();
		static dungeon_model_ptr read(const node& n);
		node write();
	private:
		int width_;
		int height_;
		std::vector<std::vector<int>> tile_map_;
	};

	/*class dungeon_controller
	{
	public:
		dungeon_controller(const node& ruleset, dungeon_model_ptr model);
	private:
	};

	class dungeon_view
	{
	public:
		dungeon_view(const dungeon_controller& controller);
	private:
		const dungeon_controller& controller_;
	};*/

	// simplified version
	class dungeon_view
	{
	public:
		dungeon_view(dungeon_model_ptr model);
		void draw(int cx, int cy) const;
	private:
		dungeon_model_ptr model_;
		graphics::texture tex_;
	};
}
