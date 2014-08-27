			//auto tex = graphics::texture::get("images/noise1.png");
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "action_process.hpp"
#include "ai_process.hpp"
#include "asserts.hpp"
#include "collision_process.hpp"
#include "engine.hpp"
#include "font.hpp"
#include "generate_cave.hpp"
#include "gui_process.hpp"
#include "json.hpp"
#include "input_process.hpp"
#include "node_utils.hpp"
#include "profile_timer.hpp"
#include "render_process.hpp"
#include "sdl_wrapper.hpp"
#include "unit_test.hpp"
#include "wm.hpp"

#define FRAME_RATE	(static_cast<int>(1000.0/60.0))

void sdl_gl_setup()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

void draw_perf_stats(engine& eng, double update_time)
{
	font::font_ptr fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
	std::stringstream ss1;
	ss1 << "Frame update time (uS): " << std::fixed << update_time;
	auto surf = font::render_shaded(ss1.str(), fnt, graphics::color(1.0f, 1.0f, 0.5f), graphics::color(0,0,0));
	auto tex = SDL_CreateTextureFromSurface(eng.get_renderer(), surf);
	SDL_Rect dst = {0, 0, surf->w, surf->h};
	SDL_RenderCopy(eng.get_renderer(), tex, NULL, &dst);
	SDL_FreeSurface(surf);
	SDL_DestroyTexture(tex);
}

/*node get_cave_params()
{
	node_builder res;
	node_builder pass;
	res.add("thresholds", 0.4);
	pass.add("iterations", 1)
		.add("thresholds", [](const node& args) { return node::from_bool(args.as_int() >= 5); })
		.add("thresholds", [](const node& args) { return node::from_bool(args.as_int() < 1); });
	res.add("passes", pass.build());
	pass.clear()
		.add("iterations", 2)
		.add("thresholds", [](const node& args) { return node::from_bool(args.as_int() >= 5); });
	res.add("passes", pass.build());
	return res.build();
}*/


SDL_Surface* make_surface_from_strings(const std::vector<std::string>& strs)
{
	std::vector<SDL_Surface*> surfaces;
	surfaces.reserve(strs.size());
	graphics::color white(255,255,255);
	int max_height = 0;
	int max_width = -1;
	font::font_ptr fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
	for(auto& s : strs) {
		auto surf = font::render(s, fnt, white);
		max_height += surf->h;
		if(surf->w > max_width) {
			max_width = surf->w;
		}
		surfaces.emplace_back(surf);
	}
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif	
	auto dst = SDL_CreateRGBSurface(0, max_width, max_height, 32, rmask, gmask, bmask, amask);
	SDL_SetSurfaceBlendMode(dst, SDL_BLENDMODE_NONE);
	ASSERT_LOG(dst != NULL, "Error creating destination surface: " << SDL_GetError());
	int height = 0;
	for(auto& surf : surfaces) {
		SDL_Rect dr = {0, height, surf->w, surf->h};
		SDL_BlitSurface(surf, NULL, dst, &dr);
		height += surf->h;
		SDL_FreeSurface(surf);
	}
	return dst;
}

SDL_Surface* make_surface_from_strings2(const std::vector<std::vector<std::string>>& strs)
{
	std::vector<SDL_Surface*> surfaces;
	surfaces.reserve(strs.size());
	graphics::color white(255, 255, 255);
	int max_height = 0;
	int max_width = -1;
	font::font_ptr fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
	for(auto& row : strs) {
		int h = -1;
		int w = 0;
		for(auto& s : row) {
			auto surf = font::render(s, fnt, white);
			if(surf->h > h) {
				h = surf->h;
			}
			w += surf->w;
			surfaces.emplace_back(surf);
		}
		if(w > max_width) {
			max_width = w;
		}
		max_height += h;
	}
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
#else
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
#endif	
	auto dst = SDL_CreateRGBSurface(0, max_width, max_height, 32, rmask, gmask, bmask, amask);
	SDL_SetSurfaceBlendMode(dst, SDL_BLENDMODE_NONE);
	ASSERT_LOG(dst != NULL, "Error creating destination surface: " << SDL_GetError());
	int height = 0;
	for(auto& surf : surfaces) {
		SDL_Rect dr = {0, height, surf->w, surf->h};
		SDL_BlitSurface(surf, NULL, dst, &dr);
		height += surf->h;
		SDL_FreeSurface(surf);
	}
	return dst;
}

std::vector<std::string> convert_map_to_strings(const map_type& m)
{
	std::vector<std::string> res;
	res.reserve(m.size());
	for(auto& row : m) {
		std::string s;
		for(auto& col : row) {
			s += convert_map_symbol_to_char(col);
		}
		res.emplace_back(s);
	}
	return res;
}

SDL_Surface* make_surface_from_map(const map_type& m)
{
	return make_surface_from_strings(convert_map_to_strings(m));
}

SDL_Surface* make_surface_from_terrain(const node& terrain_symbols, const std::vector<terrain::chunk_ptr>& chunks)
{
	std::vector<std::vector<std::string>> res;
	// scan through to calculate max sizes we'll need
	int x1 = std::numeric_limits<int>::max();
	int x2 = std::numeric_limits<int>::min();
	int y1 = std::numeric_limits<int>::max();
	int y2 = std::numeric_limits<int>::min();
	for(auto& chk : chunks) {
		if(x1 > chk->position().x - chk->width() / 2) { x1 = chk->position().x - chk->width() / 2; }
		if(x2 < chk->position().x + chk->width() / 2) { x2 = chk->position().x + chk->width() / 2; }
		if(y1 > chk->position().y - chk->height() / 2) { y1 = chk->position().y - chk->height() / 2; }
		if(y2 < chk->position().y + chk->height() / 2) { y2 = chk->position().y + chk->height() / 2; }
	}
	res.resize(y2 - y1);
	for(auto& row : res) {
		row.resize(x2 - x1);
	}
	for(auto& chk : chunks) {
		for(int y = 0; y != chk->height(); ++y) {
			for(int x = 0; x != chk->width(); ++x) {
				auto tt = chk->get_at(x, y);
				std::string sym;
				switch(tt) {
					case terrain::TerrainType::VOID: sym = terrain_symbols["void"].as_string(); break;
					case terrain::TerrainType::DIRT: sym = terrain_symbols["dirt"].as_string(); break;
					case terrain::TerrainType::GRASS: sym = terrain_symbols["grass"].as_string(); break;
					case terrain::TerrainType::SHALLOW_WATER: sym = terrain_symbols["shallow_water"].as_string(); break;
					case terrain::TerrainType::DEEP_WATER: sym = terrain_symbols["deep_water"].as_string(); break;
					case terrain::TerrainType::SHORE: sym = terrain_symbols["shore"].as_string(); break;
					case terrain::TerrainType::SAND: sym = terrain_symbols["sand"].as_string(); break;
					case terrain::TerrainType::HILL: sym = terrain_symbols["hill"].as_string(); break;
					case terrain::TerrainType::MOUNTAIN: sym = terrain_symbols["mountain"].as_string(); break;
					default:
						ASSERT_LOG(false, "Unknown terrain type: " << static_cast<int>(tt));
						break;
				}
				res[y + chk->position().y - y1 - chk->height() / 2][x + chk->position().x - x1 - chk->width() / 2] = sym;
			}
		}
	}
	return make_surface_from_strings2(res);
}

void create_player(engine& e, const point& start)
{
	entity_ptr player = std::make_shared<entity>();
	// Player component simply acts as a tag for the entity
	font::font_ptr fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
	player->get()->mask |= 1 << component::Component::PLAYER;
	player->get()->mask |= 1 << component::Component::POSITION;
	player->get()->mask |= 1 << component::Component::STATS;
	player->get()->mask |= 1 << component::Component::INPUT;
	player->get()->mask |= 1 << component::Component::SPRITE;
	player->get()->mask |= 1 << component::Component::COLLISION;
	player->get()->pos = std::make_shared<component::position>(start);
	e.set_camera(player->get()->pos);
	player->get()->stat = std::make_shared<component::stats>(10);
	player->get()->inp = std::make_shared<component::input>();
	auto surf = font::render_shaded("@", fnt, graphics::color(255,255,255), graphics::color(255,0,0));
	player->get()->spr = std::make_shared<component::sprite>(e.get_renderer(), surf);
	e.add_entity(player);

	// Create GUI (needs player stats to we stick it in here for now)
	entity_ptr gui = std::make_shared<entity>();
	gui->get()->mask |= 1 << component::Component::POSITION;
	gui->get()->mask |= 1 << component::Component::SPRITE;
	gui->get()->mask |= 1 << component::Component::STATS;
	gui->get()->mask |= 1 << component::Component::GUI;
	gui->get()->pos = std::make_shared<component::position>();
	gui->get()->spr = std::make_shared<component::sprite>(e.get_renderer(), nullptr);
	gui->get()->stat = player->get()->stat;
	e.add_entity(gui);
}

/*entity_ptr create_cave(engine& e)
{
	entity_ptr cave = std::make_shared<entity>();
	cave->get()->mask |= 1 << component::Component::SPRITE;
	cave->get()->mask |= 1 << component::Component::MAP;
	cave->get()->mask |= 1 << component::Component::COLLISION;
	cave->get()->map = std::make_shared<component::mapgrid>(json::parse_from_file("data/levels/cave/cave25.cfg"));
	cave->get()->spr = std::make_shared<component::sprite>(e.get_renderer(), make_surface_from_map(cave->get()->map->map));
	cave->get()->pos = std::make_shared<component::position>(0,0);
	e.add_entity(cave);
	return cave;
}*/

entity_ptr create_world(engine& e, const node& terrain_symbols)
{
	entity_ptr world = std::make_shared<entity>();
	world->get()->mask |= 1 << component::Component::SPRITE;
	world->get()->mask |= 1 << component::Component::MAP;
	world->get()->mask |= 1 << component::Component::COLLISION;
	world->get()->map = std::make_shared<component::mapgrid>();
	int screen_width_in_tiles  = (e.get_window().width() + e.get_tile_size().x - 1) / e.get_tile_size().x;
	int screen_height_in_tiles = (e.get_window().height() + e.get_tile_size().y - 1) / e.get_tile_size().y;
	rect area = rect::from_coordinates(-screen_width_in_tiles / 2, -screen_height_in_tiles / 2, screen_width_in_tiles / 2, screen_height_in_tiles / 2);
	auto chunks = world->get()->map->t.get_chunks_in_area(area);
	world->get()->spr = std::make_shared<component::sprite>(e.get_renderer(), make_surface_from_terrain(terrain_symbols, chunks));
	e.add_entity(world);
	return world;
}

void create_goblin(engine& e)
{
	entity_ptr mob = std::make_shared<entity>();
	// Player component simply acts as a tag for the entity
	font::font_ptr fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
	mob->get()->mask |= 1 << component::Component::ENEMY;
	mob->get()->mask |= 1 << component::Component::AI;
	mob->get()->mask |= 1 << component::Component::POSITION;
	mob->get()->mask |= 1 << component::Component::STATS;
	mob->get()->mask |= 1 << component::Component::SPRITE;
	mob->get()->mask |= 1 << component::Component::COLLISION;
	mob->get()->pos = std::make_shared<component::position>(point(23, 4));
	mob->get()->stat = std::make_shared<component::stats>(2);
	mob->get()->aip = std::make_shared<component::ai>();
	auto surf = font::render_shaded("g", fnt, graphics::color(0,96,16), graphics::color(0,0,0));
	mob->get()->spr = std::make_shared<component::sprite>(e.get_renderer(), surf);
	e.add_entity(mob);
}

node load_terrain_map(const std::string& filename)
{
	return json::parse_from_file(filename);
}

int main(int argc, char* argv[])
{
	std::vector<std::string> args;
	for(int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	int width = 800;
	int height = 600;
	for(auto it = args.begin(); it != args.end(); ++it) {
		size_t sep = it->find('=');
		std::string arg_name = *it;
		std::string arg_value;
		if(sep != std::string::npos) {
			arg_name = it->substr(0, sep);
			arg_value = it->substr(sep + 1);
		}

		if(arg_name == "--width") {
			width = boost::lexical_cast<int>(arg_value);
		} else if(arg_name == "--height") {
			height = boost::lexical_cast<int>(arg_value);
		}
	}

	if(!test::run_tests()) {
		// Just exit if some tests failed.
		exit(1);
	}

	try {
		graphics::SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
		graphics::window_manager wm;
		sdl_gl_setup();
		wm.create_window("roguelike",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		wm.set_icon("images/icon.png");
		wm.gl_init();

		bool running = true;

		font::manager font_manager;

		// Get some metrics about display size and font size to calculate how much we're showing at once.
		auto fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
		// Because this is a monospaced font, we can grab the size for one character and it will be the 
		// same for all.
		int cw, ch;
		font::get_text_size(fnt, "X", &cw, &ch);
		LOG_INFO("Tile Size: (" << cw << "," << ch << ")");

		node terrain_symbols = load_terrain_map("data/terrain.cfg")["terrain_symbols"];

		// XX engine should take the renderer as a parameter, expose it as a get function, then pass itself
		// to the update function.
		engine e(wm);
		e.set_tile_size(point(cw, ch));
		//auto cave = create_cave(e);
		//create_player(e, cave->get()->map->start);
	
		create_world(e, terrain_symbols);
		create_player(e, point(0, 0));
		create_goblin(e);
		e.add_process(std::make_shared<process::input>());
		e.add_process(std::make_shared<process::render>());
		e.add_process(std::make_shared<process::gui>());
		e.add_process(std::make_shared<process::ai>());
		e.add_process(std::make_shared<process::action>());
		// N.B. entity/map collision needs to come before entity/entity collision
		e.add_process(std::make_shared<process::em_collision>());
		e.add_process(std::make_shared<process::ee_collision>());

		while(running) {
			Uint32 cycle_start_tick = SDL_GetTicks();
			profile::timer tm;

			SDL_RenderClear(wm.get_renderer());
			running = e.update(60.0/1000.0);
			draw_perf_stats(e, tm.get_time());
			SDL_RenderPresent(wm.get_renderer());
	
			Uint32 delay = SDL_GetTicks() - cycle_start_tick;

			if(delay > FRAME_RATE) {
				//std::cerr << "CYCLE TOO LONG: " << delay << std::endl;
			} else {
				SDL_Delay(FRAME_RATE - delay);
			}
		}

	} catch(std::exception& e) {
		std::cerr << e.what();
	}
	return 0;
}
