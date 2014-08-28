#pragma once

#include "engine.hpp"
#include "node.hpp"

namespace creature
{
	void loader(const node& n);

	component_set_ptr spawn(const std::string& type, const point& pos);
}
