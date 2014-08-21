#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"

namespace graphics
{
	namespace render
	{
		struct manager
		{
			manager();
			~manager();
		};

		void set_mv_matrix(const glm::mat4& mv);
		void set_p_matrix(const glm::mat4& p);
		void blit_2d_texture(const_texture_ptr tex, float x, float y, float w, float h);

		void draw_hexagon(float x, float y, float radius);
	}
}

