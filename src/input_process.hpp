#pragma once

#include <queue>
#include "process.hpp"

namespace process
{
	class input : public process
	{
	public:
		input();
		~input();
		void update(engine& eng, double t, const entity_list& elist) override;
	private:
		bool handle_event(const SDL_Event& evt);
		std::queue<SDL_Scancode> keys_pressed_;
	};
}
