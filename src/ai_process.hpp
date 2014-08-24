#pragma once

#include "process.hpp"

namespace process
{
	class ai : public process
	{
	public:
		ai();
		~ai();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	};
}
