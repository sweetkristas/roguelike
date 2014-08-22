#include "asserts.hpp"
#include "entity.hpp"

entity::entity()
{
}

entity::~entity()
{
}

void entity::add(component_ptr component)
{
	if(components_.find(component->id()) != components_.end()) {
		LOG_WARN("Over-writing component with id: " << component->id() << " in entity "/* << name() */);
	}
	components_[component->id()] = component;
}

void entity::remove(component_ptr component)
{
	auto it = components_.find(component->id());
	ASSERT_LOG(it != components_.end(), "Unable to remove component from entity, id:'" << component->id() << "' not found.");
	components_.erase(it);
}

component_ptr entity::get(const std::string& id)
{
	auto it = components_.find(id);
	ASSERT_LOG(it != components_.end(), "Unable to get component from entity, id:'" << id << "' not found.");
	return it->second;
}
