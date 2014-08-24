#pragma once

#include <string>
#include <vector>

enum class MapSymbols
{
	DIRT,
	STONE,
	WATER,
	GRASS,

	EXIT,
	START,

	TREE, // Still on the fence about whether this should be here or not

};

typedef std::vector<std::vector<MapSymbols>> map_type;

MapSymbols convert_map_symbol(char c);
char convert_map_symbol_to_char(MapSymbols sym);
bool is_passable(MapSymbols sym);
