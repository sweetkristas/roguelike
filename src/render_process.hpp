#pragma once

#include "process.hpp"

namespace process
{
	class render : public process
	{
	public:
		render();
		~render();
		void update(engine& eng, double t, const entity_list& elist) override;
	private:
	};
}
