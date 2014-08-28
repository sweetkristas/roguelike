#include <algorithm>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <noise/noise.h>
#include "noiseutils.h"

#include "asserts.hpp"
#include "engine.hpp"
#include "random.hpp"
#include "surface.hpp"
#include "terrain_generator.hpp"

namespace terrain
{
	namespace
	{
		class tile
		{
		public:
			tile(float threshold, TerrainType tt, const std::string& name, const std::string& image) 
				: threshold_(threshold), 
				  terrain_type_(tt), 
				  name_(name) 
			{
				surface_.reset(new graphics::surface(image));
			}
			TerrainType get_terrain_type() const { return terrain_type_; }
			float get_threshold() const { return threshold_; }
			const std::string& get_name() const { return name_; }
			surface_ptr get_surface() { return surface_; }
		private:
			float threshold_;
			TerrainType terrain_type_;
			std::string name_;
			surface_ptr surface_;
		};
		inline bool operator<(const tile& lhs, const tile& rhs) { return lhs.get_threshold() < rhs.get_threshold(); }

		class terrain_data
		{
		public:
			terrain_data() {}
			void add_tile(const tile& gp)
			{
				tiles_.emplace_back(gp);
			}
			void sort_tiles() {
				std::stable_sort(tiles_.begin(), tiles_.end());
			}
			const point& get_tile_size() const { return tile_size_; }
			void set_tile_size(const point& p) { tile_size_ = p; }
			surface_ptr get_surface_at_height(float value) {				
				if(value <= tiles_.front().get_threshold()) {
					return tiles_.front().get_surface();
				} else if(value >= tiles_.back().get_threshold()) {
					return tiles_.back().get_surface();
				}
				for(auto& gp : tiles_) {
					if(value < gp.get_threshold()) {
						return gp.get_surface();
					}
				}
				ASSERT_LOG(false, "No valid terrain value found for value: " << value);
				return nullptr;
			}

		private:
			point tile_size_;
			std::vector<tile> tiles_;
		};

		terrain_data& get_terrain_data()
		{
			static terrain_data res;
			return res;
		}
	}

	chunk::chunk(const point&pos, int width, int height)
		: pos_(pos),
		  width_(width),
		  height_(height),
		  texture_(nullptr)
	{
		terrain_.resize(height);
		for(auto& row : terrain_) {
			row.resize(width);
		}
	}

	chunk::~chunk()
	{
		if(texture_) {
			SDL_DestroyTexture(texture_);
			texture_ = nullptr;
		}
	}

	float chunk::get_at(int x, int y)
	{
		ASSERT_LOG(x < width_, "x exceeds width of chunk: " << x << " >= " << width_);
		ASSERT_LOG(y < height_, "y exceeds height of chunk: " << y << " >= " << height_);
		return terrain_[y][x];
	}

	void chunk::set_at(int x, int y, float tt)
	{
		ASSERT_LOG(x < width_, "x exceeds width of chunk: " << x << " >= " << width_);
		ASSERT_LOG(y < height_, "y exceeds height of chunk: " << y << " >= " << height_);
		terrain_[y][x] = tt;
	}

	surface_ptr chunk::make_surface_from_chunk(chunk_ptr chk)
	{
		auto& cache = get_terrain_data();
		auto& tile_size = cache.get_tile_size();
		int w = tile_size.x * chk->width();
		int h = tile_size.y * chk->height();

		surface_ptr dst = std::make_shared<graphics::surface>(w, h);
		for(int y = 0; y != chk->height(); ++y) {
			for(int x = 0; x != chk->width(); ++x) {
				auto height_value = chk->get_at(x, y);
				auto& surf = cache.get_surface_at_height(height_value);
				dst->blit_scaled(surf, rect(x * tile_size.x, y * tile_size.y, tile_size.x, tile_size.y));
			}
		}
		return dst;
	}

	void chunk::draw(const engine& eng, const point& cam) const
	{
		if(!texture_) {
			texture_ = SDL_CreateTextureFromSurface(const_cast<SDL_Renderer*>(eng.get_renderer()), chunk_surface_->get());
		}
		ASSERT_LOG(texture_ != nullptr, "No texture found for chunk.");
		// Note camera isn't adjusted for tile size.
		// But it is in the same units as pos_ is;

		SDL_Rect dst = {
			(pos_.x - cam.x) * eng.get_tile_size().x - chunk_surface_->width() / 2 + eng.get_window().width() / 2,
			(pos_.y - cam.y) * eng.get_tile_size().y - chunk_surface_->height() / 2 + eng.get_window().height() / 2,
			chunk_surface_->width(),
			chunk_surface_->height()
		};
		SDL_RenderCopy(const_cast<SDL_Renderer*>(eng.get_renderer()), texture_, NULL, &dst);
	}

	std::size_t point_hash::operator()(const point& p) const
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, boost::hash_value(p.x));
		boost::hash_combine(seed, boost::hash_value(p.y));
		return seed;
	}

	terrain::terrain()
		: chunk_size_w_(16),
		  chunk_size_h_(16)
	{
	}

	void terrain::load_terrain_data(const node& n)
	{
		ASSERT_LOG(n.has_key("tile_size") && n["tile_size"].is_list() && n["tile_size"].num_elements() == 2,
			"'tile_size' attribute in terrain data file must be a two element list.");
		ASSERT_LOG(n.has_key("tiles") && n["tiles"].is_map(),
			"terrain data must have 'tiles' attribute that is a map.");
		auto& td = get_terrain_data();
		auto& tiles = n["tiles"].as_map();

		TerrainType counter = 1;
		td.set_tile_size(point(static_cast<int>(n["tile_size"][0].as_int()), static_cast<int>(n["tile_size"][1].as_int())));
		for(const auto& t : tiles) {
			const auto& name = t.first.as_string();
			ASSERT_LOG(t.second.has_key("image"), "tiles must have 'image' attribute");
			const auto& image = t.second["image"].as_string();
			ASSERT_LOG(t.second.has_key("gradient"), "tiles must have 'gradient' attribute");
			const auto& gradient = t.second["gradient"].as_float();
			td.add_tile(tile(gradient, counter++, name, image));
		}

		td.sort_tiles();
	}

	std::vector<chunk_ptr> terrain::get_chunks_in_area(const rect& r)
	{
		// We assume that chunks are placed on a grid. So that 0,0 is the co-ordinates of the 
		// center of the first chunk, the chunk one up/one right would be at chunk_size_w_,chunk_size_h_
	
		std::vector<chunk_ptr> res;
		
		// So first step, we create a new rectangle based on our grid that is normalised to cover the rectangle r.
		rect r_norm = rect::from_coordinates((r.x() - chunk_size_w_) / chunk_size_w_,
			(r.y() - chunk_size_h_) / chunk_size_h_,
			(r.x2() + chunk_size_w_) / chunk_size_w_,
			(r.y2() + chunk_size_h_) / chunk_size_h_);
		// Next iterate over the grid positions and get all the relevant chunks
		for(int y = r_norm.y(); y < r_norm.y2(); ++y) {
			for(int x = r_norm.x(); x < r_norm.x2(); ++x) {
				point pos(x * chunk_size_w_, y * chunk_size_h_);
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
		// XXX Need to add some chunk unloading logic.
		return res;
	}

	chunk_ptr terrain::generate_terrain_chunk(const point& pos)
	{
		using namespace noise;
		module::Perlin pnoise;
		const double scale = 4.0;
		pnoise.SetSeed(static_cast<int>(random::get_seed()));
		chunk_ptr nchunk = std::make_shared<chunk>(pos, chunk_size_w_, chunk_size_h_);
		for(int y = 0; y < chunk_size_h_; ++y) {
			for(int x = 0; x < chunk_size_w_; ++x) {
				auto ns = pnoise.GetValue(
					(x + pos.x - chunk_size_w_ / 2.0) / (chunk_size_w_ * scale),
					(y + pos.y - chunk_size_h_ / 2.0) / (chunk_size_h_ * scale),
					0.0);
				nchunk->set_at(x, y, static_cast<float>(ns));
			}
		}
		nchunk->set_surface(chunk::make_surface_from_chunk(nchunk));
		return nchunk;
	}

	point terrain::get_terrain_size()
	{
		return get_terrain_data().get_tile_size();
	}
}
