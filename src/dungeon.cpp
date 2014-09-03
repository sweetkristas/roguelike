#include <iterator>
#include <limits>

#include "asserts.hpp"
#include "dungeon.hpp"
#include "profile_timer.hpp"
#include "random.hpp"
#include "texture.hpp"
#include "wm.hpp"

namespace dungeon
{
	namespace
	{
		enum class Direction
		{
			NW,
			N,
			NE,
			W,
			CENTER,
			E,
			SW,
			S,
			SE,
			MAX,
		};

		const int match_anything_but = -1;

		const int min_dungeon_size = 16;
		const int max_dungeon_size = 64;

		const int any_value = 0;
		const int nil_value = 1;

		struct image
		{
			surface_ptr tex;
			int padding;
			int tile_size;
		};

		struct tile_rule
		{
			int index;
			std::vector<int> matches;
		};

		class dungeon_rule
		{
		public:
			dungeon_rule() : default_index_(-1) {}
			dungeon_rule(int def, std::vector<tile_rule>* rules) 
				: default_index_(def)
			{
				rules_.swap(*rules);
			}
			const std::vector<tile_rule>& get_rules() const { return rules_; }
			int get_default() const { return default_index_; }
		private:
			int default_index_;
			std::vector<tile_rule> rules_;
		};

		typedef std::map<int,image> level_image_map;
		level_image_map& get_dungeon_tile_image()
		{
			static level_image_map res;
			return res;
		}

		// We should replace then next two with a bi-directional map
		// One is mostly for debug though.
		typedef std::map<std::string,int> name_index_map;
		name_index_map& get_name_index_map()
		{
			static name_index_map res;
			return res;
		}
		typedef std::map<int,std::string> index_name_map;
		index_name_map& get_index_name_map()
		{
			static index_name_map res;
			return res;
		}

		typedef std::map<int, dungeon_rule> tile_rule_map;
		tile_rule_map& get_rule_map()
		{
			static tile_rule_map res;
			return res;
		}

		// Checks that the direction is one we know
		Direction get_direction_index(const std::string& dir)
		{
			if(dir == "nw" || dir == "northwest") {
				return Direction::NW;
			} else if(dir == "n" || dir == "north") {
				return Direction::N;
			} else if(dir == "ne" || dir == "northeast") {
				return Direction::NE;
			} else if(dir == "w" || dir == "west") {
				return Direction::W;
			} else if(dir == "e" || dir == "east") {
				return Direction::E;
			} else if(dir == "sw" || dir == "southwest") {
				return Direction::SW;
			} else if(dir == "S" || dir == "south") {
				return Direction::S;
			} else if(dir == "SE" || dir == "southeast") {
				return Direction::SE;
			}
			ASSERT_LOG(false, "Unrecognised direction: " << dir);
			return Direction::N;
		}

		int match_tile(const std::vector<int>& surrounds)
		{
			int max_dir = static_cast<int>(Direction::MAX);
			ASSERT_LOG(surrounds.size() == max_dir, "Surrounds is wrong size: " << surrounds.size() << " != " << max_dir);
			auto it = get_rule_map().find(surrounds[static_cast<int>(Direction::CENTER)]);
			ASSERT_LOG(it != get_rule_map().end(), 
				"Couldn't find a dungeon rule matching the value: " << static_cast<int>(Direction::CENTER));
			const auto& dr = it->second;
			auto& rules = dr.get_rules();
			
			for(auto& rule : rules) {
				bool accept = true;
				for(int n = 0; n != max_dir; ++n) {
					const int t1 = surrounds[n];
					const int t2 = rule.matches[n];
					if(t2 == any_value || t1 == t2 || t1 != -t2) {
						continue;
					}
					accept = false;
					break;
				}
				if(accept) {
					return rule.index;
				}
			}
			return dr.get_default();
		}

		image get_surface_from_level(int level)
		{
			auto it = get_dungeon_tile_image().find(level);
			if(it == get_dungeon_tile_image().end()) {
				// use default (level 0)
				it = get_dungeon_tile_image().find(0);
				ASSERT_LOG(it != get_dungeon_tile_image().end(), "Unable to find a default image to use for level.");
			}
			return it->second;
		}

		rect get_tile_area_from_index(const image& img, int index)
		{
			const int tiles_x = img.tex->width()  / (img.tile_size + img.padding);
			const int tiles_y = img.tex->height() / (img.tile_size + img.padding);
			const int x = (index % tiles_x) * (img.tile_size + img.padding);
			const int y = (index / tiles_y) * (img.tile_size + img.padding);
			return rect(x, y, img.tile_size, img.tile_size);
		}
	}

	// XXX This should all be folded into a dungeon_controller class.
	void load_rules(const node& v)
	{
		// Load the images to use on various levels.
		ASSERT_LOG(v.has_key("images") && v["images"].is_list(), 
			"dungeon rules file must have 'images' attribute that is a list.");
		for(unsigned n = 0; n != v["images"].num_elements(); ++n) {
			const auto& img = v["images"][n];
			ASSERT_LOG(img.is_map() && img.has_key("image") && img["image"].is_string(), 
				"elements in the images list of the dungeon rules must have 'image' attribute that is a string.");
			image image_data;
			image_data.tex = graphics::surface::create(img["image"].as_string());
			image_data.padding = img.has_key("padding") ? img["padding"].as_int32() : 0;
			image_data.tile_size = img["tile_size"].as_int32();
			if(img.has_key("levels")) {
				ASSERT_LOG(img["levels"].is_list(), "'levels' attribute must be a list.");
				for(auto &lvl : img["levels"].as_list()) {
					if(lvl.is_int()) {
						int val = lvl.as_int32();
						auto it = get_dungeon_tile_image().find(val);
						ASSERT_LOG(it == get_dungeon_tile_image().end(), "Overlapping images found in the dungeon rules file at " << val);
						get_dungeon_tile_image()[val] = image_data;
					} else if(lvl.is_list()) {
						ASSERT_LOG(lvl.num_elements() == 2, "Ranged values must have two elements.");
						int low = lvl[0].as_int32();
						int high = lvl[0].as_int32();
						for(int val = low; val <= high; ++val) {
							auto it = get_dungeon_tile_image().find(val);
							ASSERT_LOG(it == get_dungeon_tile_image().end(), "Overlapping images found in the dungeon rules file at " << val);
							get_dungeon_tile_image()[val] = image_data;
						}
					} else {
						ASSERT_LOG(false, "Unrecongnised type in levels list, must be int or list: " << lvl.type_as_string());
					}
				}
			} else {
				// Is default image
				auto it = get_dungeon_tile_image().find(0);
				ASSERT_LOG(it == get_dungeon_tile_image().end(), "Multiple default images found in the dungeon rules file.");
				get_dungeon_tile_image()[0] = image_data;
			}
		}

		// Load the ruleset
		ASSERT_LOG(v.has_key("rules") && v["rules"].is_map(), 
			"Dungeon rules file must have a 'rules' attribute that is a map.");
		auto& rule_map = v["rules"].as_map();

		// add a mapping for any value
		get_name_index_map()["any"] = any_value;
		get_index_name_map()[any_value] = "any";

		// Add a mapping for NIL(i.e. border) tiles. 
		get_name_index_map()["NIL"] = nil_value;
		get_index_name_map()[nil_value] = "NIL";

		int count = nil_value + 1;

		// Pre-scan rules to generate the names of the tiles.
		for(auto& rule : rule_map) {
			ASSERT_LOG(rule.second.has_key("default_index") && rule.second.has_key("rules"),
				"rules should have 'default_index' and 'rules' attributes.");
			const auto& name = rule.first.as_string();
			get_name_index_map()[name] = count;
			get_index_name_map()[count] = name;
			++count;
		}
		for(auto& rule : rule_map) {
			// First element is the name of the tile. Second element is the details
			const auto& name = rule.first.as_string();
				auto it = get_name_index_map().find(name);
			ASSERT_LOG(it != get_name_index_map().end(), "Something bad happened " << name << " wasn't found in our internal list.");
			int name_index = it->second;
			const int default_index = rule.second["default_index"].as_int32();
			std::vector<tile_rule> tile_rules;
			for(auto& r : rule.second["rules"].as_list()) {
				ASSERT_LOG(r.has_key("index") && r.has_key("map"), "rules must have 'index' and 'map' attributes.");
				tile_rules.push_back(tile_rule());
				tile_rules.back().index = r["index"].as_int32();
				tile_rules.back().matches.resize(static_cast<int>(Direction::MAX), 0);
				// Add center tile
				tile_rules.back().matches[static_cast<int>(Direction::CENTER)] = name_index;
				for(auto& rmap : r["map"].as_map()) {
					auto value = rmap.second.as_string();
					bool negate = false;
					if(value[0] == '!') {
						// negation condition.
						negate = true;
						value = value.substr(1);
					}
					auto it = get_name_index_map().find(value);
					ASSERT_LOG(it != get_name_index_map().end(), "Didn't find definition for tile name: " << value);
					Direction dir = get_direction_index(rmap.first.as_string());
					tile_rules.back().matches[static_cast<int>(dir)] = it->second * (negate ? match_anything_but : 1);
				}
			}
			get_rule_map()[name_index] = dungeon_rule(default_index, &tile_rules);
		}
	}

	dungeon_model::dungeon_model()
	{
	}

	dungeon_model_ptr dungeon_model::generate()
	{
		auto dung = std::make_shared<dungeon_model>();
		dung->width_ = generator::get_uniform_int<int>(min_dungeon_size, max_dungeon_size);
		dung->height_ = generator::get_uniform_int<int>(min_dungeon_size, max_dungeon_size);
		dung->tile_map_.resize(dung->height_);
		for(auto& row : dung->tile_map_) {
			row.resize(dung->width_);
		}

		for(int y = 0; y != dung->height_; ++y) {
			for(int x = 0; x != dung->width_; ++x) {
				//int r = generator::get_uniform_int<int>(2, static_cast<int>(get_index_name_map().size()-1));
				//auto it = get_index_name_map().begin();
				//std::advance(it, r);
				dung->tile_map_[y][x] =  get_name_index_map()["wall"];//it->first;
			}
		}
		return dung;
	}

	dungeon_model_ptr dungeon_model::read(const node& n)
	{
		return dungeon_model_ptr();
	}

	node dungeon_model::write()
	{
		return node();
	}

	int dungeon_model::get_at(int x, int y)
	{
		//ASSERT_LOG(y < height_, "y value exceeds height of dungeon model: " << y << " >= " << height_);
		//ASSERT_LOG(y < height_, "x value exceeds width of dungeon model: " << x << " >= " << width_);
		if(y >= height_ || x >= width_ || x < 0 || y < 0) {
			return nil_value;
		}
		return tile_map_[y][x];
	}

	dungeon_view::dungeon_view(dungeon_model_ptr model)
		: model_(model)
	{
		profile::manager pman("dungeon_view constructor");

		int level = 0;

		image& img = get_surface_from_level(level);

		std::vector<int> surrounds(9);
		surface_ptr dest = graphics::surface::create(model->width()*img.tile_size, model_->height()*img.tile_size);

		for(int y = 0; y != model_->height(); ++y) {
			for(int x = 0; x != model_->width(); ++x) {
				surrounds[0] = model_->get_at(x-1, y-1);
				surrounds[1] = model_->get_at(x+0, y-1);
				surrounds[2] = model_->get_at(x+1, y-1);
				surrounds[3] = model_->get_at(x-1, y+0);
				surrounds[4] = model_->get_at(x+0, y+0);
				surrounds[5] = model_->get_at(x+1, y+0);
				surrounds[6] = model_->get_at(x-1, y+1);
				surrounds[7] = model_->get_at(x+0, y+1);
				surrounds[8] = model_->get_at(x+1, y+1);
				rect area = get_tile_area_from_index(img, match_tile(surrounds));
				dest->blit_scaled(img.tex, area, rect(x*img.tile_size, y*img.tile_size, img.tile_size, img.tile_size));
			}
		}
		tex_ = graphics::texture(dest, graphics::TextureFlags::NONE);
	}

	void dungeon_view::draw(int cx, int cy) const
	{
		auto& wm = graphics::window_manager::get_main_window();
		const int src_x = wm.width() > tex_.width() 
			? 0 
			: cx < wm.width()/2 
				? 0
				: cx > tex_.width() - wm.width()/2 ? tex_.width()-wm.width() : cx - wm.width()/2;
		const int src_y = wm.height() > tex_.height() 
			? 0 
			: cy < wm.height()/2 
				? 0
				: cy > tex_.height() - wm.height()/2 ? tex_.height()-wm.height() : cy - wm.height()/2;
		const int src_w = wm.width() > tex_.width() ? tex_.width() : wm.width();
		const int src_h = wm.height() > tex_.height() ? tex_.height() : wm.height();

		const int dst_x = wm.width() > tex_.width() ? (wm.width() - tex_.width())/2 : 0;
		const int dst_y = wm.height() > tex_.height() ? (wm.height() - tex_.height())/2 : 0;
		const int dst_w = wm.width() > tex_.width() ? tex_.width() : wm.width();
		const int dst_h = wm.height() > tex_.height() ? tex_.height() : wm.height();

		tex_.blit(rect(src_x, src_y, src_w, src_h), 
			rect(dst_x, dst_y, dst_w, dst_h));
	}
}
