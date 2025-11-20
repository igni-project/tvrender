#ifndef IO_TVRENDER_H
#define IO_TVRENDER_H 1

#include "tvr_internal.h"

#include <stdint.h>

/* Translate versioned TVrender opcode to internal opcode */

int tvr_int_opcode(
	uint16_t opcode,
	int tvr_v,
	uint16_t *int_opcode
);

/* Misc. requests */

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

int recv_tvr_mesh_bind_tex(
	int fd,
	struct tvr_msg_mesh_bind_tex *msg
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

int recv_tvr_texture_write(
	int fd,
	struct tvr_msg_texture_write *msg
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

int recv_tvr_pov_look_at(
	int fd,
	struct tvr_msg_pov_look_at *msg
);

int recv_tvr_pov_set_fov(
	int fd,
	struct tvr_msg_pov_set_fov *msg
);

#endif

