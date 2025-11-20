/* gfxenv.h - Graphical Environment */

#ifndef RENDER_GFXENV_H
#define RENDER_GFXENV_H 1

#include "opengl.h"

struct vertex
{
	float pos[3];
	float normal[3];
	float texCoord[2];
};

struct gfxenv_scene
{
	int fd;
	/* TVrender protocol version */ 
	int tvr_ver;
};

struct gfxenv
{
	struct gfxenv_scene *scenes;
	unsigned int scene_count;
	unsigned int scene_limit;

	union
	{
		struct gl_gfxenv gl;
	} data;
};

extern int (*p_gfxenv_render)(struct gfxenv *, GLFWwindow *);
extern void (*p_destroy_gfxenv)(struct gfxenv);
extern int (*p_gfxenv_new_client)(struct gfxenv *, int);
extern int (*p_gfxenv_exec)(struct gfxenv *, unsigned int);

/* Misc. gfx. env. functions */

int create_gfxenv(
	struct gfxenv *gfxenv,
	GLFWwindow *window
);

int create_gfxenv_gl(
	struct gfxenv *gfxenv,
	GLFWwindow *window
);

int gfxenv_render_gl(
	struct gfxenv *gfxenv,
	GLFWwindow *window
);

int gfxenv_render_na(
	struct gfxenv *gfxenv,
	GLFWwindow *window
);

void destroy_gfxenv_gl(
	struct gfxenv gfxenv
);

void destroy_gfxenv_na(
	struct gfxenv gfxenv
);

int gfxenv_new_client_gl(
	struct gfxenv *gfxenv,
	int fd
);

int gfxenv_new_client_na(
	struct gfxenv *gfxenv,
	int fd
);

int gfxenv_exec_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_exec_na(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_del_scene_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_del_scene_na(
	struct gfxenv *gfxenv,
	unsigned int scene
);

/* Request handlers */

int gfxenv_set_tvr_version(
	struct gfxenv *gfxenv,
	unsigned int scene
);

/* Vertex Buffer */

int gfxenv_create_vertex_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_destroy_vertex_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

/* Index Buffer */

int gfxenv_create_index_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_destroy_index_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

/* Mesh */

int gfxenv_create_mesh_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_mesh_bind_index_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_mesh_bind_mat_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_mesh_bind_vertex_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_mesh_set_loc_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_mesh_set_rot_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_mesh_set_scale_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_destroy_mesh_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_remove_mesh_manual_gl(
	struct gl_scene *gls,
	int idx
);

int gfxenv_mesh_bind_mat_manual_gl(
	struct gl_scene *gls,
	int mesh_idx,
	int mat_idx
);

void gfxenv_mesh_apply_tf_gl(struct gl_mesh *mesh);

/* Texture */

int gfxenv_create_texture_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_destroy_texture_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

/* Material */

int gfxenv_create_material_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_material_bind_texture_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_destroy_material_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

/* POV */

int gfxenv_set_pov_loc_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_set_pov_rot_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_pov_look_at_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

int gfxenv_set_pov_fov_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
);

#endif

