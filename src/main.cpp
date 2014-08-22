			//auto tex = graphics::texture::get("images/noise1.png");
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "asserts.hpp"
#include "engine.hpp"
#include "font.hpp"
#include "generate_cave.hpp"
#include "node_utils.hpp"
#include "profile_timer.hpp"
#include "render.hpp"
#include "render_text.hpp"
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

bool process_events()
{
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
			case SDL_MOUSEWHEEL:
				break;
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN:
				if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					return false;
				} else if(e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
				} else if(e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
				} else if(e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
				} else if(e.key.keysym.scancode == SDL_SCANCODE_UP) {
				}
				break;
		}
	}

	return true;
}

namespace
{
	int frame_render_time;
	int frame_processing_time;
}

void draw_ui(const glm::mat4& ortho_camera)
{
	graphics::render::set_p_matrix(ortho_camera);
	graphics::render::set_mv_matrix(glm::mat4(1.0f));

	std::stringstream ss1, ss2;
	ss1 << "Frame draw time (uS): " << std::fixed << frame_render_time;
	auto r = graphics::render::text::quick_draw(0, 600-40, ss1.str(), "SourceCodePro-Regular.ttf", 14, graphics::color(1.0f, 1.0f, 0.5f));
	ss2 << "Frame process time (uS): " << std::fixed << frame_processing_time;
	graphics::render::text::quick_draw(0, r.y2(), ss2.str(), "SourceCodePro-Regular.ttf", 14, graphics::color(1.0f, 1.0f, 0.5f));
}

node get_cave_params()
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
}

void draw_xxx()
{
	//static std::vector<std::string> cave = generator::cave(32, 32, get_cave_params());
	static std::vector<std::string> cave = generator::cave_fixed_param(256, 256);

	rect r = rect(0, 0);
	for(auto& s : cave) {
		r = graphics::render::text::quick_draw(r.x(), r.y2(), s, "SourceCodePro-Regular.ttf", 20, graphics::color(1.0f, 1.0f, 1.0f));
	}
}

void draw_stuff(const glm::mat4& camera)
{
	graphics::render::set_p_matrix(camera);
	graphics::render::set_mv_matrix(glm::mat4(1.0f));

	//graphics::render::draw_hexagon(0, 0, 1);
	draw_xxx();
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

		glm::mat4 ui_camera = glm::ortho(0.0f, float(width), float(height), 0.0f);
		glm::mat4 scene_camera = glm::ortho(0.0f, float(width), float(height), 0.0f);;

		graphics::render::manager render_manager;
		font::manager font_manager;

		engine e;
		entity_ptr player = std::make_shared<entity>();
		player->add(std::make_shared<position_component>(0, 0));
		// display component should take a texture used to display the player.
		player->add(std::make_shared<display_component>());
		e.add_entity(player);

		while(running) {
			Uint32 cycle_start_tick = SDL_GetTicks();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			running = process_events();
			frame_processing_time = SDL_GetTicks() - cycle_start_tick;

			// Draw here
			//graphics::render::set_p_matrix(ortho_camera);
			//graphics::render::set_mv_matrix(glm::mat4(1.0f));
			//auto tex = graphics::texture::get("images/noise1.png");
			//graphics::render::blit_2d_texture(tex, 0, 0, 800, 600);
			draw_stuff(scene_camera);
			
			frame_render_time = SDL_GetTicks() - frame_processing_time - cycle_start_tick;
			draw_ui(ui_camera);

			wm.swap();

			Uint32 delay = SDL_GetTicks() - cycle_start_tick;
			render_acc += delay;
			render_cnt++;
			Uint32 current_time = SDL_GetTicks();
			if(current_time - start_time >= 1000) {
				std::cerr << "Average processing time: " << double(render_acc) / double(render_cnt) << " ms " << render_acc << " : " << render_cnt << std::endl;

				start_time = current_time;
				render_cnt = 0;
				render_acc = 0;
			}

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
