#pragma once

#include <memory>

#include "node.hpp"

class parameter;
typedef std::shared_ptr<parameter> parameter_ptr;

// Base class parameter implementation, doubles as fixed value parameter.
class parameter
{
public:
	explicit parameter(float value=0) : value_(value) {}
	explicit parameter(const node& n);
	virtual ~parameter() {}
	virtual float get_value(float t) { return value_; }
	static parameter_ptr create(const node& n);
private:
	float value_;
};
