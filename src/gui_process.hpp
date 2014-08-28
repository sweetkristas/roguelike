#pragma once

#include "process.hpp"

namespace process
{
	class gui : public process
	{
	public:
		gui();
		~gui();
		void update(engine& eng, double t, const entity_list& elist) override;
	private:
	};
}
