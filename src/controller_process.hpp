#pragma once

#include "process.hpp"

namespace process
{
	class controller : public process
	{
	public:
		controller();
		~controller();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	};
}
