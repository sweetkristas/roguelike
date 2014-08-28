#pragma once

#include <random>

namespace generator
{
	std::size_t get_seed();
	void set_seed(std::size_t seed);
	std::size_t generate_seed();

	std::mt19937& get_random_engine();

	template<typename T>
	T get_uniform_int(T mn, T mx)
	{
		auto& re = get_random_engine();
		std::uniform_int_distribution<T> uniform_dist(mn, mx);
		return uniform_dist(re);
	}

	template<typename T>
	T get_uniform_real(T mn, T mx)
	{
		auto& re = get_random_engine();
		std::uniform_real_distribution<T> uniform_dist(mn, mx);
		return uniform_dist(re);
	}
}
