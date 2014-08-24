#pragma once

#include <memory>

#include "component.hpp"

class entity
{
public:
	entity();
	~entity();
	component::component_set_ptr get() { return components_; }
private:
	component::component_set_ptr components_;
};

typedef std::shared_ptr<entity> entity_ptr;
