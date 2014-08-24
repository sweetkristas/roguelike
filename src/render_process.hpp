#pragma once

#include "process.hpp"

namespace process
{
	class render : public process
	{
	public:
		render();
		~render();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	private:
	};
}
