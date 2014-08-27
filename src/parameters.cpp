#include <random>

#include "asserts.hpp"
#include "parameters.hpp"

namespace
{
	class random_parameter : public parameter
	{
	public:
		random_parameter(const node& n) : min_(0.0f), max_(1.0f) {
			seed_ = std::default_random_engine()();
			if(n.has_key("min")) {
				min_ = n["min"].as_float();
			}
			if(n.has_key("max")) {
				max_ = n["max"].as_float();
			}
		}
		float get_value(float t) {
			static std::mt19937 random_engine(seed_);
			std::uniform_real_distribution<float> uniform_dist(min_, max_);
			return uniform_dist(random_engine);
		}
	private:
		unsigned seed_;
		float min_;
		float max_;
	};
}

parameter::parameter(const node& n)
{
	value_ = n.as_float();
}

parameter_ptr parameter::create(const node& n)
{
	// If is numeric asume a fixed value.
	if(n.is_numeric()) {
		return std::make_shared<parameter>(n);
	}
	const std::string& type = n["type"].as_string();
	if(type == "fixed") {
		return std::make_shared<parameter>(n["value"]);
	} else if(type == "random") {
		return std::make_shared<random_parameter>(n);
	}
	ASSERT_LOG(false, "Unknown type for parameter: " << type);
	return nullptr;
}