#ifndef IO_TVRENDER_H
#define IO_TVRENDER_H 1

#include <stdint.h>

struct tvr_msg_set_version
{
	int32_t version;
};

/* Vertex Buffer */

struct tvr_msg_vertex_buffer_create
{
	int32_t vb_id;
	uint64_t size;
	void *data;
};

struct tvr_msg_vertex_buffer_destroy
{
	int32_t vb_id;
};

/* Index Buffer */

struct tvr_msg_index_buffer_create
{
	int32_t ib_id;
	uint64_t size;
	void *data;
};

struct tvr_msg_index_buffer_destroy
{
	int32_t ib_id;
};

/* Mesh */

struct tvr_msg_mesh_create
{
	int32_t mesh_id;
	int32_t vb_id;
	int32_t ib_id;
};

struct tvr_msg_mesh_bind_mat
{
	int32_t mesh_id;
	int32_t mat_id;
};

struct tvr_msg_mesh_set_loc
{
	int32_t mesh_id;
	float x, y, z;
};

struct tvr_msg_mesh_set_rot
{
	int32_t mesh_id;
	float x, y, z;
};

struct tvr_msg_mesh_set_scale
{
	int32_t mesh_id;
	float x, y, z;
};

struct tvr_msg_mesh_destroy
{
	int32_t mesh_id;
};

/* Texture */

struct tvr_msg_texture_create
{
	int32_t tex_id;
	int32_t width;
	int32_t height;
	int8_t channels;
	void *data;
};

struct tvr_msg_texture_destroy
{
	int32_t tex_id;
};

/* Material */

struct tvr_msg_material_create
{
	int32_t mat_id;
};

struct tvr_msg_material_bind_texture
{
	int32_t mat_id;
	int32_t tex_id;
	int8_t pass;
};

struct tvr_msg_material_destroy
{
	int32_t mat_id;
};

/* Viewpoint */

struct tvr_msg_pov_set_loc
{
	float x, y, z;
};

struct tvr_msg_pov_set_rot
{
	float x, y, z;
};

struct tvr_msg_pov_set_fov
{
	float fov;
};

int recv_tvr_set_version(
	int fd,
	struct tvr_msg_set_version *msg
);

/* vertex buffer requests */

int recv_tvr_vertex_buffer_create(
	int fd,
	struct tvr_msg_vertex_buffer_create *msg
);

int recv_tvr_vertex_buffer_destroy(
	int fd,
	struct tvr_msg_vertex_buffer_destroy *msg
);

/* index buffer requests */

int recv_tvr_index_buffer_create(
	int fd,
	struct tvr_msg_index_buffer_create *msg
);

int recv_tvr_index_buffer_destroy(
	int fd,
	struct tvr_msg_index_buffer_destroy *msg
);

/* mesh requests */

int recv_tvr_mesh_create(
	int fd,
	struct tvr_msg_mesh_create *msg
);

int recv_tvr_mesh_bind_mat(
	int fd,
	struct tvr_msg_mesh_bind_mat *msg
);

int recv_tvr_mesh_set_loc(
	int fd,
	struct tvr_msg_mesh_set_loc *msg
);

int recv_tvr_mesh_set_rot(
	int fd,
	struct tvr_msg_mesh_set_rot *msg
);

int recv_tvr_mesh_set_scale(
	int fd,
	struct tvr_msg_mesh_set_scale *msg
);

int recv_tvr_mesh_destroy(
	int fd,
	struct tvr_msg_mesh_destroy *msg
);

/* texture requests */

int recv_tvr_texture_create(
	int fd,
	struct tvr_msg_texture_create *msg
);

int recv_tvr_texture_destroy(
	int fd,
	struct tvr_msg_texture_destroy *msg
);

/* material requests */

int recv_tvr_material_create(
	int fd,
	struct tvr_msg_material_create *msg
);
int recv_tvr_material_bind_texture(
	int fd,
	struct tvr_msg_material_bind_texture *msg
);

int recv_tvr_material_destroy(
	int fd,
	struct tvr_msg_material_destroy *msg
);

/* pov requests */

int recv_tvr_pov_set_loc(
	int fd,
	struct tvr_msg_pov_set_loc *msg
);

int recv_tvr_pov_set_rot(
	int fd,
	struct tvr_msg_pov_set_rot *msg
);

int recv_tvr_pov_set_fov(
	int fd,
	struct tvr_msg_pov_set_fov *msg
);

#endif

