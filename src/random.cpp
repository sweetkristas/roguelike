#include "asserts.hpp"
#include "random.hpp"

namespace generator
{
	namespace
	{
		bool seed_set = false;
		std::size_t seed_internal = 0;
	}

	std::mt19937& get_random_engine()
	{
		static std::mt19937 random_engine(seed_internal);
		ASSERT_LOG(seed_set, "No seed set");
		return random_engine;
	}

	std::size_t get_seed()
	{
		return seed_internal;
	}

	void set_seed(std::size_t seed)
	{
		seed_internal = seed;
		seed_set = true;
	}

	std::size_t generate_seed()
	{
		seed_internal = std::default_random_engine()();
		seed_set = true;
		return seed_internal;
	}
}
