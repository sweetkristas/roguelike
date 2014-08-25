#pragma once

#include <bitset>
#include <memory>
#include <string>
#include <vector>

#include "SDL.h"

#include "color.hpp"
#include "geometry.hpp"
#include "map.hpp"
#include "node.hpp"


typedef std::bitset<64> component_id;

namespace component
{
	// XXX Todo thing of a cleaner way of doing this with bitsets.
	enum class Component
	{
		POSITION,
		SPRITE,
		STATS,
		AI,
		INPUT,
		LIGHTS,
		MAP,
		// tag only values must go at end.
		PLAYER,
		ENEMY,
		GUI,
		COLLISION,
		MAX_COMPONENTS,
	};
	static_assert(static_cast<int>(Component::MAX_COMPONENTS) <= 64, "Maximum number of components must be less than 64. If you need more consider a vector<bool> solution.");

	inline component_id operator<<(int value, const Component& rhs) 
	{
		return component_id(value << static_cast<unsigned long long>(rhs));
	}
	inline component_id genmask(const Component& lhs)
	{
		return 1 << lhs;
	}

	class component
	{
	public:
		explicit component(Component id) : id_(component_id(static_cast<unsigned long long>(id))) {}
		virtual ~component() {}
		component_id id() const { return id_; }
	private:
		const component_id id_;
	};

	typedef std::shared_ptr<component> component_ptr;

	struct position : public component
	{
		explicit position(int xx, int yy) : component(Component::POSITION), p(xx, yy), last_p() {}
		explicit position(const point& pp) : component(Component::POSITION), p(pp), last_p() {}
		point p;
		point last_p;
	};

	struct sprite : public component
	{
		sprite(SDL_Renderer* renderer, SDL_Surface* surf);
		void update_texture(SDL_Renderer* renderer, SDL_Surface* surf);
		~sprite();
		SDL_Texture* tex;
		int width;
		int height;
	};

	struct stats : public component
	{
		stats(int hh) : component(Component::STATS), health(hh) {}
		int health;
	};

	struct ai : public component
	{
		ai() : component(Component::AI) {}
		// XXX Need to add some data
	};

	struct input : public component
	{
		input() : component(Component::INPUT) {}
		enum class Action {
			none,
			up,
			down,
			left,
			right,
			use,
			attack,
		} action;
	};

	struct point_light
	{
		point_light(int xx, int yy, const graphics::color& cc, float att)
			: x(xx), y(yy), color(cc), attenuation(att) 
		{}
		int x;
		int y;
		graphics::color color;
		float attenuation;
	};

	struct lights : public component
	{
		lights();
		~lights();
		// XXX These should in some sort of quadtree like structure.
		std::vector<point_light> light_list;
		SDL_Texture* tex;		
	};

	struct mapgrid : public component
	{
		mapgrid(const node& n);
		point start;
		// XXX need to know where exists are linked to.
		std::vector<point> exits;
		map_type map;
	};

	struct component_set
	{
		component_id mask;
		std::shared_ptr<position> pos;
		std::shared_ptr<sprite> spr;
		std::shared_ptr<stats> stat;
		std::shared_ptr<ai> aip;
		std::shared_ptr<input> inp;
		std::shared_ptr<mapgrid> map;
		bool is_player() { return (mask & genmask(Component::PLAYER)) == genmask(Component::PLAYER); }
	};
	typedef std::shared_ptr<component_set> component_set_ptr;
}

