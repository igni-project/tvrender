#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H 1

#include <cglm/cglm.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

struct gl_display
{
	unsigned int program;
};

struct gl_viewpoint
{
	mat4 proj;
	mat4 view;
	vec3 loc;
	vec3 fwd;
	vec3 up;
};

struct gl_texture
{
	GLuint tex;

	int *bound_mesh_ids;
	int *bound_mesh_passes;
	unsigned int bound_mesh_count;
	unsigned int bound_mesh_lim;

	int *bound_mat_ids;
	int *bound_passes;
	unsigned int mat_bind_count;
	unsigned int mat_bind_lim;
};

struct gl_material
{
	char has_albedo;
	int albedo_id;
};

struct gl_mesh
{
	GLuint vertex_array;
	GLuint albedo;
	mat4 tf;

	vec3 rot;
	vec3 loc;
	vec3 scale;
};

struct gl_buffer
{
	GLuint buf;

	int *bound_mesh_ids;
	unsigned int bound_mesh_count;
	unsigned int bound_mesh_lim;
};

struct gl_scene
{
	struct gl_buffer *vertex_buffers;
	int *vertex_buffer_ids;
	unsigned int vertex_buffer_count;
	unsigned int vertex_buffer_lim;

	struct gl_buffer *index_buffers;
	int *index_buffer_ids;
	unsigned int index_buffer_count;
	unsigned int index_buffer_lim;

	struct gl_mesh *meshes;
	int *mesh_ids;
	unsigned int mesh_count;
	unsigned int mesh_lim;

	struct gl_texture *textures;
	int *texture_ids;
	unsigned int texture_count;
	unsigned int texture_lim;

	struct gl_material *mats;
	int *mat_ids;
	unsigned int mat_count;
	unsigned int mat_lim;
};

struct gl_gfxenv
{
	unsigned int program;
	GLuint error_tex;
	struct gl_viewpoint pov;

	GLuint tf_ubo;
	GLuint proj_ubo;
	GLuint view_ubo;

	struct gl_scene *scenes;
};

int gl_load_shader(const char *path, GLenum shader_type, unsigned int *shader);
int destroy_gl_scene(struct gl_scene gls);
int destroy_gl_texture(struct gl_texture tex);

void framebuffer_size_callback_gl(GLFWwindow *window, int width, int height);
int get_screen_width();
int get_screen_height();

#endif

