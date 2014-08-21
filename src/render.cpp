#include "color.hpp"
#include "filesystem.hpp"
#include "graphics.hpp"
#include "render.hpp"
#include "shaders.hpp"

namespace graphics
{
	namespace render
	{
		namespace
		{
			shader::program_object_ptr simple_shader;
			shader::const_actives_map_iterator simple_a_position_it;
			shader::const_actives_map_iterator simple_m_matrix_it;
			shader::const_actives_map_iterator simple_v_matrix_it;
			shader::const_actives_map_iterator simple_p_matrix_it;
			shader::const_actives_map_iterator simple_u_color_it;

			shader::program_object_ptr tex2d_shader;
			shader::const_actives_map_iterator tex2d_u_texmap_it;
			shader::const_actives_map_iterator tex2d_a_position_it;
			shader::const_actives_map_iterator tex2d_a_texcoord_it;
			shader::const_actives_map_iterator tex2d_mv_matrix_it;
			shader::const_actives_map_iterator tex2d_p_matrix_it;

			shader::program_object_ptr poly_shader;
			shader::const_actives_map_iterator poly_u_color_it;
			shader::const_actives_map_iterator poly_a_position_it;

			const int num_generic_vbo = 2;
			GLuint generic_vbo[num_generic_vbo];

			glm::mat4 mv_matrix;
			glm::mat4 p_matrix;
		}

		manager::manager()
		{
			tex2d_shader.reset(new shader::program_object("texture_shader_2d",
				shader::shader(GL_VERTEX_SHADER, "texture_2d_vert", sys::read_file("data/texture_2d.vert")),
				shader::shader(GL_FRAGMENT_SHADER, "texture_2d_frag", sys::read_file("data/texture_2d.frag"))));
			tex2d_u_texmap_it = tex2d_shader->get_uniform_iterator("u_tex_map");
			tex2d_a_position_it = tex2d_shader->get_attribute_iterator("a_position");
			tex2d_a_texcoord_it = tex2d_shader->get_attribute_iterator("a_texcoord");
			tex2d_mv_matrix_it = tex2d_shader->get_uniform_iterator("mv_matrix");
			tex2d_p_matrix_it = tex2d_shader->get_uniform_iterator("p_matrix");
			//tex2d_shader->make_active();

			poly_shader.reset(new shader::program_object("poly_shader_2d",
				shader::shader(GL_VERTEX_SHADER, "poly_2d_vert", sys::read_file("data/simple_poly.vert")),
				shader::shader(GL_FRAGMENT_SHADER, "poly_2d_frag", sys::read_file("data/simple_poly.frag"))));
			poly_u_color_it = poly_shader->get_uniform_iterator("u_color");
			poly_a_position_it = poly_shader->get_attribute_iterator("a_position");
			
			simple_shader.reset(new shader::program_object("simple_color_shader",
				shader::shader(GL_VERTEX_SHADER, "simple_color_vert", sys::read_file("data/simple_color.vert")),
				shader::shader(GL_FRAGMENT_SHADER, "simple_color_frag", sys::read_file("data/simple_color.frag"))));
			simple_a_position_it = simple_shader->get_attribute_iterator("a_position");
			simple_m_matrix_it = simple_shader->get_uniform_iterator("model_matrix");
			simple_v_matrix_it = simple_shader->get_uniform_iterator("view_matrix");
			simple_p_matrix_it = simple_shader->get_uniform_iterator("projection_matrix");
			simple_u_color_it = simple_shader->get_uniform_iterator("u_color");

			glGenBuffers(num_generic_vbo, generic_vbo);
		}

		manager::~manager()
		{
			glDeleteBuffers(num_generic_vbo, generic_vbo);
		}

		void set_mv_matrix(const glm::mat4& mv)
		{
			mv_matrix = mv;
		}

		void set_p_matrix(const glm::mat4& p)
		{
			p_matrix = p;
		}

		void blit_2d_texture(const_texture_ptr tex, float x, float y, float w, float h)
		{
			GLfloat vc_array[] = {
				x,     y,     0.0f, 0.0f,
				x + w, y,     1.0f, 0.0f,
				x,     y + h, 0.0f, 1.0f,
				x + w, y + h, 1.0f, 1.0f,
			};
			tex2d_shader->make_active();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->id());
			glUniform1i(tex2d_u_texmap_it->second.location, 0);

			glUniformMatrix4fv(tex2d_mv_matrix_it->second.location, 1, GL_FALSE, glm::value_ptr(mv_matrix));
			glUniformMatrix4fv(tex2d_p_matrix_it->second.location, 1, GL_FALSE, glm::value_ptr(p_matrix));

			glBindBuffer(GL_ARRAY_BUFFER, generic_vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vc_array), vc_array, GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(tex2d_a_position_it->second.location);
			glVertexAttribPointer(
				tex2d_a_position_it->second.location, // The attribute we want to configure
				2,                    // size
				GL_FLOAT,             // type
				GL_FALSE,             // normalized?
				sizeof(GLfloat) * 4,  // stride
				0					  // array buffer offset
			);

			glEnableVertexAttribArray(tex2d_a_texcoord_it->second.location);
			glVertexAttribPointer(
				tex2d_a_texcoord_it->second.location, // The attribute we want to configure
				2,                            // size : U+V => 2
				GL_FLOAT,                     // type
				GL_FALSE,                     // normalized?
				sizeof(GLfloat) * 4,          // stride
				reinterpret_cast<const GLvoid*>(sizeof(GLfloat) * 2) // array buffer offset
			);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDisableVertexAttribArray(tex2d_a_position_it->second.location);
			glDisableVertexAttribArray(tex2d_a_texcoord_it->second.location);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void draw_hexagon(float x, float y, float radius)
		{
			std::vector<glm::vec3> varray;
			for(int n = 0; n != 6; ++n) {
				float angle = static_cast<float>(2.0 * M_PI / 6.0 * n);
				varray.emplace_back(x + radius * cos(angle), y + radius * sin(angle), 0.0f);
			}

			simple_shader->make_active();
			glUniform4fv(simple_u_color_it->second.location, 1, graphics::color(255, 255, 0).as_gl_color());
			glUniformMatrix4fv(simple_m_matrix_it->second.location, 1, GL_FALSE, glm::value_ptr(mv_matrix));
			glUniformMatrix4fv(simple_v_matrix_it->second.location, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
			glUniformMatrix4fv(simple_p_matrix_it->second.location, 1, GL_FALSE, glm::value_ptr(p_matrix));

			glEnableVertexAttribArray(simple_a_position_it->second.location);
			glVertexAttribPointer(
				simple_a_position_it->second.location, // The attribute we want to configure
				3,                    // size
				GL_FLOAT,             // type
				GL_FALSE,             // normalized?
				0,					  // stride
				&varray[0]			  // array buffer offset
				);
			glDrawArrays(GL_TRIANGLE_FAN, 0, varray.size());
			glDisableVertexAttribArray(simple_a_position_it->second.location);
		}
	}
}
