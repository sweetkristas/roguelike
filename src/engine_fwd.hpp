#pragma once

#include <memory>
#include <vector>

class engine;

namespace component
{
	struct component_set;
}
typedef std::shared_ptr<component::component_set> component_set_ptr;

typedef std::vector<component_set_ptr> entity_list;
