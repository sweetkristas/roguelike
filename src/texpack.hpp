#pragma once

#include "texture.hpp"

namespace graphics
{
	// inp is a list of input surfaces
	struct texture_pack
	{
		texture_pack(graphics::texture t, std::vector<std::pair<surface_ptr,rect>>* rl)
			: tex(t) {
			if(rl) {
				rect_list.swap(*rl);
			}
		}
		graphics::texture tex;
		std::vector<std::pair<surface_ptr,rect>> rect_list;
	};

	class packer
	{
	public:
		typedef std::vector<texture_pack>::iterator iterator;
		typedef std::vector<texture_pack>::const_iterator const_iterator;
		packer(const std::vector<surface_ptr> inp, int max_length);
		iterator begin() { return outp.begin(); }
		iterator end() { return outp.end(); }
		const_iterator begin() const { return outp.begin(); }
		const_iterator end() const { return outp.end(); }
	private:
		std::vector<texture_pack> outp;
	};
}
