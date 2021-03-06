#pragma once

#include "process.hpp"

namespace process
{
	class ee_collision : public process
	{
	public:
		ee_collision();
		~ee_collision();
		void update(engine& eng, double t, const entity_list& elist) override;
	};

	class em_collision : public process
	{
	public:
		em_collision();
		~em_collision();
		void update(engine& eng, double t, const entity_list& elist) override;
	};
}
