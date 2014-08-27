#pragma once

#include "process.hpp"

namespace process
{
	class action : public process
	{
	public:
		action();
		~action();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	private:
	};
}
