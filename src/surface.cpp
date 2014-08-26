#include "SDL_image.h"

#include "asserts.hpp"
#include "surface.hpp"

namespace graphics
{
	surface::surface(const std::string& fname)
	{
		auto surf = IMG_Load(fname.c_str());
		ASSERT_LOG(surf_ != NULL, "Failed to load image: " << fname << " : " << IMG_GetError());
		surf_.reset(surf, [](SDL_Surface* surf){
			SDL_FreeSurface(surf);
		});
	}

	surface::surface(SDL_Surface* surf)
	{
		ASSERT_LOG(surf != NULL, "Surface passed in was null.");
		surf_.reset(surf, [](SDL_Surface* surf) {
			SDL_FreeSurface(surf);
		});
	}

	surface::~surface()
	{
	}
}
