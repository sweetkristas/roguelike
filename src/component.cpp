#include "component.hpp"

component::component(const std::string& id)
	: id_(id)
{
}

component::~component()
{
}

position_component::position_component(int x, int y)
	: component("position"), 
	  x_(x),
	  y_(y)
{
}

position_component::~position_component()
{
}

display_component::display_component()
	: component("display")
{
}

display_component::~display_component()
{
}
