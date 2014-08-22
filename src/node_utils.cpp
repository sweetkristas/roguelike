#include "node_utils.hpp"

node_builder& node_builder::add_value(const std::string& name, const node& value)
{
	attr_[node(name)].emplace_back(value);
	return *this;
}

node_builder& node_builder::set_value(const std::string& name, const node& value)
{
	node key(name);
	attr_.erase(key);
	attr_[key].emplace_back(value);
	return *this;
}

node node_builder::build()
{
	std::map<node, node> res;
	for(auto& i : attr_) {
		if(i.second.size() == 1) {
			res[i.first] = i.second[0];
		} else {
			res[i.first] = node(&i.second);
		}
	}
	return node(&res);
}

node_builder& node_builder::clear()
{
	attr_.clear();
	return *this;
}
