#pragma once

#include <vector>

#include "entity.hpp"
#include "system.hpp"

class engine
{
public:
	engine();
	~engine();
	
	void add_entity(entity_ptr e);
	void remove_entity(entity_ptr e);

	void add_system(isystem_ptr s);
	void remove_system(isystem_ptr s);

	void update(double time);
private:
	std::vector<entity_ptr> entity_list_;
	std::vector<isystem_ptr> systems_list_;
};
