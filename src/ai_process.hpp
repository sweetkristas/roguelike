#pragma once

#include "process.hpp"

namespace process
{
	class ai : public process
	{
	public:
		ai();
		~ai();
		void update(engine& eng, double t, const entity_list& elist) override;
	private:
		bool handle_event(const SDL_Event& evt);
		bool should_update_;
		int update_turns_;
	};
}
