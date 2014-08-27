#pragma once

#include <memory>
#include <unordered_map>

#include "color.hpp"
#include "geometry.hpp"

namespace terrain
{
	enum class TerrainType {
		VOID,
		DIRT,
		GRASS,
		SHALLOW_WATER,
		DEEP_WATER,
		SHORE,
		SAND,
		HILL,
		MOUNTAIN,
	};

	class chunk
	{
	public:
		chunk(const point& pos, int width, int height);
		void set_at(int x, int y, TerrainType tt);
		TerrainType get_at(int x, int y);
		int width() const { return width_; }
		int height() const { return height_; }
		const point& position() const { return pos_; }
	private:
		point pos_;
		int width_;
		int height_;
		std::vector<std::vector<TerrainType>> terrain_;
	};
	typedef std::shared_ptr<chunk> chunk_ptr;

	struct point_hash
	{
		std::size_t operator()(const point& p) const;
	};

	class gradient_point
	{
	public:
		gradient_point(float threshold, TerrainType tt) : threshold_(threshold), terrain_type_(tt) {}
		TerrainType get_terrain_type() const { return terrain_type_; }
		float get_threshold() const { return threshold_; }
	private:
		float threshold_;
		TerrainType terrain_type_;
	};
	inline bool operator<(const gradient_point& lhs, const gradient_point& rhs) { return lhs.get_threshold() < rhs.get_threshold(); }

	class terrain
	{
	public:
		terrain();
		//terrain(const node& n);

		void add_gradient_point(const gradient_point& gp);
		TerrainType get_terrain_at_height(float value);
		
		// Find all the chunks which are in the given area, including partials.
		// Will generate chunks as needed for complete coverage.
		std::vector<chunk_ptr> get_chunks_in_area(const rect& r);

		// Pos is the worldspace position.
		chunk_ptr generate_terrain_chunk(const point& pos);

		//node write();
	private:
		// Seed to use to generate terrain.
		unsigned seed_;
		int chunk_size_w_;
		int chunk_size_h_;
		std::unordered_map<point, chunk_ptr, point_hash> chunks_;
		std::vector<gradient_point> gradient_points_;
	};
}