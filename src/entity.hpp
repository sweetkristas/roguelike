#pragma once

#include <map>

#include "component.hpp"

class entity
{
public:
	entity();
	~entity();
	void add(component_ptr);
	void remove(component_ptr);
	component_ptr get(const std::string& id);
private:
	std::map<std::string, component_ptr> components_;
};

typedef std::shared_ptr<entity> entity_ptr;
