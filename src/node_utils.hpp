#pragma once

#include "node.hpp"


class node_builder
{
public:
	template<typename T> node_builder& add(const std::string& name, const T& value)
	{
		return add_value(name, node(value));
	}

	template<typename T> node_builder& add(const std::string& name, T& value)
	{
		return add_value(name, node(value));
	}

	template<typename T> node_builder& set(const std::string& name, const T& value)
	{
		return set_value(name, node(value));
	}

	template<typename T> node_builder& set(const std::string& name, T& value)
	{
		return set_value(name, node(value));
	}

	node build();
	node_builder& clear();
private:
	node_builder& add_value(const std::string& name, const node& value);
	node_builder& set_value(const std::string& name, const node& value);

	std::map<node, std::vector<node>> attr_;
};


template<> inline node_builder& node_builder::add(const std::string& name, const node& value)
{
	return add_value(name, value);
}

template<> inline node_builder& node_builder::add(const std::string& name, node& value)
{
	return add_value(name, value);
}
