#pragma once

#include "process.hpp"

namespace process
{
	class gui : public process
	{
	public:
		gui();
		~gui();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	private:
	};
}
