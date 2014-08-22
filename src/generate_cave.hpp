#pragma once

#include <string>
#include <vector>

#include "node.hpp"

namespace generator
{
	std::vector<std::string> cave(unsigned width, unsigned height, const node& params);

	std::vector<std::string> cave_fixed_param(unsigned width, unsigned height);
}