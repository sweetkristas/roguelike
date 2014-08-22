#include <algorithm>

#include "engine.hpp"

engine::engine()
{
}

engine::~engine()
{
}

void engine::add_entity(entity_ptr e)
{
	entity_list_.emplace_back(e);
}

void engine::remove_entity(entity_ptr e1)
{
	entity_list_.erase(std::remove_if(entity_list_.begin(), entity_list_.end(), 
		[&e1](entity_ptr e2) { return e1 == e2; }), entity_list_.end());
}

void engine::add_system(isystem_ptr s)
{
	systems_list_.emplace_back(s);
	std::stable_sort(systems_list_.begin(), systems_list_.end(), [](const isystem_ptr& lhs, const isystem_ptr& rhs){
		return lhs->get_priority() < rhs->get_priority();
	});
}

void engine::remove_system(isystem_ptr s)
{
	systems_list_.erase(std::remove_if(systems_list_.begin(), systems_list_.end(), 
		[&s](isystem_ptr isp) { return isp == s; }), systems_list_.end());
}

void engine::update(double time)
{
	for(auto& s : systems_list_) {
		s->update(time);
	}
}
