#pragma once

#include <memory>
#include <unordered_map>

#include "color.hpp"
#include "node.hpp"
#include "geometry.hpp"
#include "surface.hpp"

class engine;

namespace terrain
{
	typedef unsigned TerrainType;

	class chunk;
	typedef std::shared_ptr<chunk> chunk_ptr;

	class chunk
	{
	public:
		chunk(const point& pos, int width, int height);
		~chunk();
		void set_at(int x, int y, float tt);
		float get_at(int x, int y);
		int width() const { return width_; }
		int height() const { return height_; }
		const point& position() const { return pos_; }
		void set_surface(surface_ptr surf) { chunk_surface_ = surf; }
		surface_ptr get_surface() { return chunk_surface_; }
		static surface_ptr make_surface_from_chunk(chunk_ptr chk);
		void draw(const engine& eng, const point& cam) const;
	private:
		point pos_;
		int width_;
		int height_;
		std::vector<std::vector<float>> terrain_;
		// should make this a texture.
		surface_ptr chunk_surface_;
		mutable SDL_Texture* texture_;
	};
	typedef std::shared_ptr<chunk> chunk_ptr;

	struct point_hash
	{
		std::size_t operator()(const point& p) const;
	};

	class terrain
	{
	public:
		terrain();
		//terrain(const node& n);

		// Find all the chunks which are in the given area, including partials.
		// Will generate chunks as needed for complete coverage.
		std::vector<chunk_ptr> get_chunks_in_area(const rect& r);

		// Pos is the worldspace position.
		chunk_ptr generate_terrain_chunk(const point& pos);

		static void load_terrain_data(const node& n);
		static point get_terrain_size();
		//node write();
	private:
		int chunk_size_w_;
		int chunk_size_h_;
		std::unordered_map<point, chunk_ptr, point_hash> chunks_;
	};
}