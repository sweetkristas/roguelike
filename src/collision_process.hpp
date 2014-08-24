#pragma once

#include "process.hpp"

namespace process
{
	class ee_collision : public process
	{
	public:
		ee_collision();
		~ee_collision();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	};

	class em_collision : public process
	{
	public:
		em_collision();
		~em_collision();
		void update(engine& eng, double t, const std::vector<entity_ptr>& elist);
	};
}
