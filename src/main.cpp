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
#include "button.hpp"
#include "collision_process.hpp"
#include "component.hpp"
#include "creature.hpp"
#include "dialog.hpp"
#include "engine.hpp"
#include "font.hpp"
#include "generate_cave.hpp"
#include "gui_elements.hpp"
#include "gui_process.hpp"
#include "json.hpp"
#include "input_process.hpp"
#include "label.hpp"
#include "node_utils.hpp"
#include "profile_timer.hpp"
#include "random.hpp"
#include "render_process.hpp"
#include "surface.hpp"
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
	auto surf = font::render_shaded(ss1.str(), fnt, graphics::color(1.0f, 1.0f, 0.5f), graphics::color(0, 0, 0));
	auto tex = SDL_CreateTextureFromSurface(eng.get_renderer(), surf);
	SDL_Rect dst = {0, 0, surf->w, surf->h};
	SDL_RenderCopy(eng.get_renderer(), tex, NULL, &dst);
	SDL_FreeSurface(surf);
	SDL_DestroyTexture(tex);
}

void create_player(engine& e, const point& start)
{
	component_set_ptr player = std::make_shared<component::component_set>(100);
	// Player component simply acts as a tag for the entity
	//font::font_ptr fnt = font::get_font("SourceCodePro-Regular.ttf", 20);
	player->mask |= component::genmask(component::Component::PLAYER);
	player->mask |= component::genmask(component::Component::POSITION);
	player->mask |= component::genmask(component::Component::STATS);
	player->mask |= component::genmask(component::Component::INPUT);
	player->mask |= component::genmask(component::Component::SPRITE);
	player->mask |= component::genmask(component::Component::COLLISION);
	player->pos = std::make_shared<component::position>(start);
	e.set_camera(player->pos->pos);
	player->stat = std::make_shared<component::stats>();
	player->stat->health = 10;
	player->inp = std::make_shared<component::input>();
	//auto surf = std::make_shared<graphics::surface>(font::render_shaded("@", fnt, graphics::color(255,255,255), graphics::color(255,0,0)));
	auto surf = std::make_shared<graphics::surface>("images/spritely_fellow.png");
	player->spr = std::make_shared<component::sprite>(surf);
	e.add_entity(player);

	// Create GUI (needs player stats to we stick it in here for now)
	component_set_ptr guic = std::make_shared<component::component_set>(900);
	guic->mask |= component::genmask(component::Component::POSITION);
	guic->mask |= component::genmask(component::Component::SPRITE);
	guic->mask |= component::genmask(component::Component::STATS);
	guic->mask |= component::genmask(component::Component::GUI);
	guic->pos = std::make_shared<component::position>();
	guic->spr = std::make_shared<component::sprite>();
	guic->gui = std::make_shared<component::gui_component>();
	auto l = std::make_shared<gui::label>(rectf(0.0f, 0.0f), gui::Justify::H_CENTER|gui::Justify::V_CENTER, "Press Me!");
	l->set_size(24);
	auto w = std::make_shared<gui::button>(rectf(0.0f, -0.1f), gui::Justify::H_CENTER|gui::Justify::BOTTOM, [](){std::cerr<<"pressed\n";}, l);

	auto dlg = std::make_shared<gui::dialog>(rectf(0.25f, 0.15f, 0.5f, 0.7f), gui::Justify::LEFT|gui::Justify::TOP);
	dlg->add_widget(w);
	//guic->gui->widgets.push_back(dlg);
	guic->stat = player->stat;
	e.add_entity(guic);
}

component_set_ptr create_world(engine& e)
{
	component_set_ptr world = std::make_shared<component::component_set>(0);
	world->mask |= component::genmask(component::Component::MAP);
	world->mask |= component::genmask(component::Component::COLLISION);
	world->map = std::make_shared<component::mapgrid>();
	const point& cam = e.get_camera();
	const int screen_width_in_tiles = (e.get_window().width() + e.get_tile_size().x - 1) / e.get_tile_size().x;
	const int screen_height_in_tiles = (e.get_window().height() + e.get_tile_size().y - 1) / e.get_tile_size().y;
	rect area = rect::from_coordinates(-screen_width_in_tiles / 2 + cam.x,
		-screen_height_in_tiles / 2 + cam.y,
		screen_width_in_tiles / 2 + cam.x,
		screen_height_in_tiles / 2 + cam.y);
	auto chunks = world->map->t.get_chunks_in_area(area);
	e.add_entity(world);
	return world;
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

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

		bool running = true;

		// XXX Try and load a save file here, including random seed. If no save file we generate a new seed
		generator::generate_seed();

		font::manager font_manager;
		graphics::texture::manager texture_manager(wm.get_renderer());

		terrain::terrain::load_terrain_data(json::parse_from_file("data/terrain.cfg"));

		creature::loader(json::parse_from_file("data/creatures.cfg"));

		gui::section::manager gui_manager(wm.get_renderer(), json::parse_from_file("data/gui.cfg"));

		// XX engine should take the renderer as a parameter, expose it as a get function, then pass itself
		// to the update function.
		engine e(wm);
		e.set_tile_size(terrain::terrain::get_terrain_size());
		//auto cave = create_cave(e);
		//create_player(e, cave->get()->map->start);

		create_player(e, point(0, 0));
		create_world(e);		
		e.add_entity(creature::spawn("goblin", point(1, 1)));

		e.add_process(std::make_shared<process::input>());
		e.add_process(std::make_shared<process::render>());
		e.add_process(std::make_shared<process::gui>());
		e.add_process(std::make_shared<process::ai>());
		e.add_process(std::make_shared<process::action>());
		// N.B. entity/map collision needs to come before entity/entity collision
		e.add_process(std::make_shared<process::em_collision>());
		e.add_process(std::make_shared<process::ee_collision>());

		graphics::texture darkness(wm.width(), wm.height(), graphics::TextureFlags::NO_CACHE | graphics::TextureFlags::TARGET);
		graphics::texture light("images/light.png", graphics::TextureFlags::NONE);
		darkness.set_blend(graphics::BlendMode::BLEND);
		light.set_blend(graphics::BlendMode::MODULATE);
		SDL_SetRenderTarget(wm.get_renderer(), darkness.get());
		SDL_SetRenderDrawColor(wm.get_renderer(), 16, 16, 32, 224);
		SDL_RenderClear(wm.get_renderer());
		light.blit(rect((wm.width()-e.get_tile_size().x*4)/2, (wm.height()-e.get_tile_size().y*4)/2, e.get_tile_size().x*4, e.get_tile_size().y*4));

		SDL_SetRenderTarget(wm.get_renderer(), NULL);
		SDL_SetRenderDrawColor(wm.get_renderer(), 255, 255, 255, 255);
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
