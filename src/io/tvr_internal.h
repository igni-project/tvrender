/* tvr_internal.h - Internal TVrender protocol data structures */

#ifndef IO_TVR_INTERNAL_H
#define IO_TVR_INTERNAL_H 1

#include <stddef.h>
#include <stdint.h>

/* Internal Opcodes */
enum
{
	/* Miscellaneous requests */

	TVR_INT_OPC_NULL = 0,
	TVR_INT_OPC_SET_VERSION,

	/* VERTEX BUFFER Requests */

	TVR_INT_OPC_VERTEX_BUFFER_CREATE,
	TVR_INT_OPC_VERTEX_BUFFER_DESTROY,

	/* INDEX BUFFER Requests */

	TVR_INT_OPC_INDEX_BUFFER_CREATE,
	TVR_INT_OPC_INDEX_BUFFER_DESTROY,

	/* MESH Requests */

	TVR_INT_OPC_MESH_CREATE,
	TVR_INT_OPC_MESH_BIND_MAT,
	TVR_IND_OPC_MESH_BIND_TEX,
	TVR_INT_OPC_MESH_SET_LOC,
	TVR_INT_OPC_MESH_SET_ROT,
	TVR_INT_OPC_MESH_SET_SCALE,
	TVR_INT_OPC_MESH_DESTROY,

	/* TEXTURE Requests */

	TVR_INT_OPC_TEXTURE_CREATE,
	TVR_INT_OPC_TEXTURE_DESTROY,

	/* MATERIAL Requests */

	TVR_INT_OPC_MATERIAL_CREATE,
	TVR_INT_OPC_MATERIAL_BIND_TEXTURE,
	TVR_INT_OPC_MATERIAL_DESTROY,

	/* POV Requests */

	TVR_INT_OPC_POV_SET_LOC,
	TVR_INT_OPC_POV_SET_ROT,
	TVR_INT_OPC_POV_LOOK_AT,
	TVR_INT_OPC_POV_SET_FOV
};


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

struct tvr_msg_mesh_bind_tex
{
	int32_t mesh_id;
	int32_t tex_id;
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

struct tvr_msg_texture_write
{
	int32_t tex_id;
	uint32_t x, y, w, h;
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

struct tvr_msg_pov_look_at
{
	float x, y, z;
};

struct tvr_msg_pov_set_fov
{
	float fov;
};


#endif

