#include <algorithm>
#include <map>

#include "asserts.hpp"
#include "gui_elements.hpp"
#include "texpack.hpp"

namespace gui
{
	namespace section
	{
		namespace 
		{
			typedef std::map<std::string, graphics::texture> section_map;
			section_map& get_section_map()
			{
				static section_map res;
				return res;
			}
		}

		manager::manager(SDL_Renderer* renderer, const node& n)
		{
			SDL_RendererInfo info;
			int res = SDL_GetRendererInfo(renderer, &info);
			ASSERT_LOG(res == 0, "Failed to get renderer info: " << SDL_GetError());

			ASSERT_LOG(n.has_key("sections") && n["sections"].is_list(), 
				"Must be 'sections' attribute in gui file which is a list.");
			std::vector<std::pair<std::string, surface_ptr>> surfs;
			for(auto& s : n["sections"].as_list()) {
				surfs.emplace_back(std::make_pair(s["name"].as_string(), std::make_shared<graphics::surface>(s["image"].as_string())));
			}
			for(auto& vtex : graphics::packer(surfs, info.max_texture_width, info.max_texture_height)) {
				for(auto& tex : vtex) {
					get_section_map()[tex.get_name()] = tex;
				}
			}
		}

		manager::~manager()
		{
		}

		graphics::texture get(const std::string& name)
		{
			auto it = get_section_map().find(name);
			ASSERT_LOG(it != get_section_map().end(), "Unable to find a gui texture with name: " << name);
			return it->second;
		}
	}
}
