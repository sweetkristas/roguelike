			//auto tex = graphics::texture::get("images/noise1.png");
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ai_process.hpp"
#include "asserts.hpp"
#include "collision_process.hpp"
#include "controller_process.hpp"
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
	gui->get()->pos = std::make_shared<component::position>(0, 0);
	gui->get()->spr = std::make_shared<component::sprite>(e.get_renderer(), nullptr);
	gui->get()->stat = player->get()->stat;
	e.add_entity(gui);
}

entity_ptr create_cave(engine& e)
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
	mob->get()->pos = std::make_shared<component::position>(23, 4);
	mob->get()->stat = std::make_shared<component::stats>(2);
	mob->get()->ai = std::make_shared<component::ai>();
	auto surf = font::render_shaded("g", fnt, graphics::color(0,96,16), graphics::color(0,0,0));
	mob->get()->spr = std::make_shared<component::sprite>(e.get_renderer(), surf);
	e.add_entity(mob);
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
		Uint32 start_time = SDL_GetTicks();
		uint64_t render_acc = 0;
		int render_cnt = 0;

		font::manager font_manager;

		// Get some metrics about display size and font size to calculate how much we're showing at once.
		auto fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
		// Because this is a monospaced font, we can grab the size for one character and it will be the 
		// same for all.
		int cw, ch;
		font::get_text_size(fnt, "X", &cw, &ch);
		LOG_INFO("Tile Size: (" << cw << "," << ch << ")");

		// XX engine should take the renderer as a parameter, expose it as a get function, then pass itself
		// to the update function.
		engine e(wm);
		auto cave = create_cave(e);
		create_player(e, cave->get()->map->start);
		create_goblin(e);
		e.add_process(std::make_shared<process::movement>());
		e.add_process(std::make_shared<process::input>());
		e.add_process(std::make_shared<process::render>());
		e.add_process(std::make_shared<process::controller>());
		e.add_process(std::make_shared<process::gui>());
		e.add_process(std::make_shared<process::ai>());
		e.add_process(std::make_shared<process::em_collision>());
		e.add_process(std::make_shared<process::ee_collision>());
		e.set_tile_size(point(cw, ch));

		while(running) {
			Uint32 cycle_start_tick = SDL_GetTicks();
			profile::timer tm;

			SDL_RenderClear(wm.get_renderer());
			running = e.update(60.0/1000.0);
			Uint32 update_time = SDL_GetTicks() - cycle_start_tick;
			draw_perf_stats(e, tm.get_time());
			SDL_RenderPresent(wm.get_renderer());
	
			//draw_ui(ui_camera);

			Uint32 delay = SDL_GetTicks() - cycle_start_tick;
			/*render_acc += delay;
			render_cnt++;
			if(SDL_GetTicks() - start_time >= 1000) {
				std::cerr << "Average processing time: " << double(render_acc) / double(render_cnt) << " ms " << render_acc << " : " << render_cnt << std::endl;

				start_time = SDL_GetTicks();
				render_cnt = 0;
				render_acc = 0;
			}*/

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
