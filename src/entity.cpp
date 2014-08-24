#include "asserts.hpp"
#include "entity.hpp"

entity::entity()
{
	components_ = std::make_shared<component::component_set>();
}

entity::~entity()
{
}
