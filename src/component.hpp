#pragma once

#include <memory>
#include <string>

class component
{
public:
	explicit component(const std::string& id);
	virtual ~component();
	const std::string& id() const { return id_; }
private:
	const std::string id_;
};

typedef std::shared_ptr<component> component_ptr;

class position_component : component
{
public:
	explicit position_component(int x, int y);
	virtual ~position_component();
private:
	int x_;
	int y_;
};

class display_component : component
{
public:
	display_component();
	virtual ~display_component();
private:
	//display_object disp_;
};
