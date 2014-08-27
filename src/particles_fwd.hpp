#pragma once

#include <memory>
#include <vector>

namespace particle
{
	class particle_system_manager;

	class particle_system;
	typedef std::shared_ptr<particle_system> particle_system_ptr;
	
	class emitter;
	typedef std::shared_ptr<emitter> emitter_ptr;
	
	class affector;
	typedef std::shared_ptr<affector> affector_ptr;
}
