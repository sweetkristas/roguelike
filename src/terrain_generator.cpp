#include <algorithm>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <noise/noise.h>
#include "noiseutils.h"

#include "asserts.hpp"
#include "terrain_generator.hpp"

namespace terrain
{
	chunk::chunk(const point&pos, int width, int height)
		: pos_(pos),
		  width_(width),
		  height_(height)
	{
		terrain_.resize(height);
		for(auto& row : terrain_) {
			row.resize(width);
		}
	}

	TerrainType chunk::get_at(int x, int y)
	{
		ASSERT_LOG(x < width_, "x exceeds width of chunk: " << x << " >= " << width_);
		ASSERT_LOG(y < height_, "y exceeds height of chunk: " << y << " >= " << height_);
		return terrain_[y][x];
	}

	void chunk::set_at(int x, int y, TerrainType tt)
	{
		ASSERT_LOG(x < width_, "x exceeds width of chunk: " << x << " >= " << width_);
		ASSERT_LOG(y < height_, "y exceeds height of chunk: " << y << " >= " << height_);
		terrain_[y][x] = tt;
	}

	std::size_t point_hash::operator()(const point& p) const
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, boost::hash_value(p.x));
		boost::hash_combine(seed, boost::hash_value(p.y));
		return seed;
	}

	terrain::terrain()
		: seed_(0),
		  chunk_size_w_(16),
		  chunk_size_h_(16)
	{
		add_gradient_point(gradient_point(-1.0f, TerrainType::DEEP_WATER));
		add_gradient_point(gradient_point(-0.25f, TerrainType::SHALLOW_WATER));
		add_gradient_point(gradient_point(0.0f, TerrainType::SHORE));
		add_gradient_point(gradient_point(0.0625f, TerrainType::SAND));
		add_gradient_point(gradient_point(0.125f, TerrainType::GRASS));
		add_gradient_point(gradient_point(0.375f, TerrainType::DIRT));
		add_gradient_point(gradient_point(0.5f, TerrainType::HILL));
		add_gradient_point(gradient_point(0.75f, TerrainType::MOUNTAIN));
		std::stable_sort(gradient_points_.begin(), gradient_points_.end());
	}

	void terrain::add_gradient_point(const gradient_point& gp)
	{
		gradient_points_.emplace_back(gp);
	}

	TerrainType terrain::get_terrain_at_height(float value)
	{
		if(value <= gradient_points_.front().get_threshold()) {
			return gradient_points_.front().get_terrain_type();
		} else if(value >= gradient_points_.back().get_threshold()) {
			return gradient_points_.back().get_terrain_type();
		}
		for(auto& gp : gradient_points_) {
			if(value < gp.get_threshold()) {
				return gp.get_terrain_type();
			}
		}
		ASSERT_LOG(false, "No valid terrain value found for value: " << value);
		return TerrainType::VOID;
	}

	std::vector<chunk_ptr> terrain::get_chunks_in_area(const rect& r)
	{
		// We assume that chunks are placed on a grid. So that 0,0 is the co-ordinates of the 
		// center of the first chunk, the chunk one up/one right would be at chunk_size_w_,chunk_size_h_
	
		std::vector<chunk_ptr> res;
		
		// So first step, we create a new rectangle based on our grid that is normalised to cover the rectangle r.
		rect r_norm = rect::from_coordinates((r.x() / chunk_size_w_)*chunk_size_w_, 
			(r.y() / chunk_size_h_)*chunk_size_h_, 
			((r.x2() + chunk_size_w_) / chunk_size_w_)*chunk_size_w_,
			((r.y2() + chunk_size_h_) / chunk_size_h_)*chunk_size_h_);
		// Next iterate over the grid positions and get all the relevant chunks
		for(int y = r_norm.y(); y < r_norm.y2(); y += chunk_size_h_) {
			for(int x = r_norm.x(); x < r_norm.x2(); x += chunk_size_w_) {
				point pos(x, y);
				auto it = chunks_.find(pos);
				if(it == chunks_.end()) {
					auto nchunk = generate_terrain_chunk(pos);
					chunks_[pos] = nchunk;
					res.emplace_back(nchunk);
				} else {
					res.emplace_back(it->second);
				}				
			}
		}
		return res;
	}

	chunk_ptr terrain::generate_terrain_chunk(const point& pos)
	{
		using namespace noise;
		module::Perlin pnoise;
		pnoise.SetSeed(seed_);
		utils::NoiseMap height_map;
		utils::NoiseMapBuilderPlane height_map_builder;
		height_map_builder.SetSourceModule(pnoise);
		height_map_builder.SetDestNoiseMap(height_map);
		height_map_builder.SetDestSize(16, 16);
		const float lx = pos.x - chunk_size_w_ / 2.0f;
		const float ux = lx + static_cast<float>(chunk_size_w_);
		const float ly = pos.y - chunk_size_h_ / 2.0f;
		const float uy = ly + static_cast<float>(chunk_size_h_);
		height_map_builder.SetBounds(lx, ux, ly, uy);
		height_map_builder.Build();
		chunk_ptr nchunk = std::make_shared<chunk>(pos, chunk_size_w_, chunk_size_h_);
		for(int y = 0; y < height_map.GetHeight(); ++y) {
			for(int x = 0; x < height_map.GetWidth(); ++x) {
				nchunk->set_at(x, y, get_terrain_at_height(height_map.GetValue(x, y)));
			}
		}
		return nchunk;
	}
}
