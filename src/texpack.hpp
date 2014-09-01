#pragma once

#include "texture.hpp"

namespace graphics
{
	typedef std::pair<std::string, surface_ptr> surface_pair;
	typedef std::vector<surface_pair> surface_pair_list;

	class packer
	{
	public:
		typedef std::vector<std::vector<graphics::texture>>::iterator iterator;
		typedef std::vector<std::vector<graphics::texture>>::const_iterator const_iterator;
		packer(const surface_pair_list& inp, int max_width, int max_height);
		iterator begin() { return outp.begin(); }
		iterator end() { return outp.end(); }
		const_iterator begin() const { return outp.begin(); }
		const_iterator end() const { return outp.end(); }
	private:
		std::vector<std::vector<graphics::texture>> outp;
	};
}
