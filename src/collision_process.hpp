#pragma once

#include "process.hpp"

namespace process
{
	class collision : public process
	{
	public:
		collision();
		~collision();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	};
}
