attribute vec2 a_position;
attribute vec2 a_texcoord;
varying vec2 v_texcoord;
uniform mat4 p_matrix;
uniform mat4 mv_matrix;

void main()
{
	v_texcoord = a_texcoord;
	gl_Position = mv_matrix * p_matrix * vec4(a_position, 0.0, 1.0);
}
