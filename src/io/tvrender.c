#include "tvrender.h"
#include "../debug/print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int recv_tvr_set_version(
	int fd,
	struct tvr_msg_set_version *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: TVrender version (32-bit) */
	recv_status = recv(fd, &msg->version, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive protocol version");
		return -1;
	}

	db_print_vb("| TVrender protocol version: %i\n", msg->version);

	return 0;
}

/* vertex buffer requests */

int recv_tvr_vertex_buffer_create(
	int fd,
	struct tvr_msg_vertex_buffer_create *msg
)
{
	int recv_status;
	size_t size;
	uintptr_t vb_recv_offset = 0;
	
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: Vertex buffer ID (32-bit) */
	recv_status = recv(fd, &msg->vb_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive vertex buffer ID");
		return -1;
	}

	db_print_vb("| Vertex buffer ID: %i\n", msg->vb_id);

	/* Field: Vertex buffer size (64-bit) */
	recv_status = recv(fd, &msg->size, 8, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive vertex buffer size");
		return -1;
	}

	db_print_vb("| Vertex buffer size: %i\n", msg->size);

	/* Vertex buffer data */
	msg->data = malloc(msg->size);
	if (!msg->data)
	{
		perror("Failed to allocate space for index buffer");
		return -1;
	}

	while (vb_recv_offset < msg->size)
	{
		recv_status = recv(
			fd,
			msg->data + vb_recv_offset,
			msg->size - vb_recv_offset,
			0
		);

		if (recv_status <= 0)
		{
			perror("Failed to receive vertex buffer data");
			return -1;
		}

		vb_recv_offset += recv_status;
	}

	return 0;
}

int recv_tvr_vertex_buffer_destroy(
	int fd,
	struct tvr_msg_vertex_buffer_destroy *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: Vertex buffer ID (32-bit) */
	recv_status = recv(fd, &msg->vb_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive vertex buffer ID");
		return -1;
	}

	db_print_vb("| Vertex buffer ID: %i\n", msg->vb_id);

	return 0;
}

/* index buffer requests */

int recv_tvr_index_buffer_create(
	int fd,
	struct tvr_msg_index_buffer_create *msg
)
{
	int recv_status;
	uintptr_t ib_recv_offset = 0;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: Index buffer ID (32-bit) */
	recv_status = recv(fd, &msg->ib_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive index buffer ID");
		return -1;
	}

	db_print_vb("| Index buffer ID: %i\n", msg->ib_id);

	/* Field: Index buffer size (64-bit) */
	recv_status = recv(fd, &msg->size, 8, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive index buffer size");
		return -1;
	}

	db_print_vb("| Index buffer size: %i\n", msg->size);

	/* Index buffer data */
	msg->data = malloc(msg->size);
	if (!msg->data)
	{
		perror("Failed to allocate space for index buffer");
		return -1;
	}

	while (ib_recv_offset < msg->size)
	{
		recv_status = recv(
			fd,
			msg->data + ib_recv_offset,
			msg->size - ib_recv_offset,
			0
		);

		if (recv_status <= 0)
		{
			perror("Failed to receive index buffer data");
			return -1;
		}

		ib_recv_offset += recv_status;
	}

	return 0;
}

int recv_tvr_index_buffer_destroy(
	int fd,
	struct tvr_msg_index_buffer_destroy *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: Index buffer ID (32-bit) */
	recv_status = recv(fd, &msg->ib_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive index buffer ID");
		return -1;
	}

	/* getting an insane value for no reason */
	db_print_vb("| Index buffer ID: %i\n", msg->ib_id);

	return 0;
}

/* mesh requests */

int recv_tvr_mesh_create(
	int fd,
	struct tvr_msg_mesh_create *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: mesh ID (32-bit) */
	recv_status = recv(fd, &msg->mesh_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive mesh ID");
		return -1;
	}

	db_print_vb("| Mesh ID: %i\n", msg->mesh_id);

	/* Field: Vertex buffer ID (32-bit) */
	recv_status = recv(fd, &msg->vb_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive vertex buffer ID");
		return -1;
	}

	db_print_vb("| Vertex buffer ID: %i\n", msg->vb_id);

	/* Field: Index buffer ID (32-bit) */
	recv_status = recv(fd, &msg->ib_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive index buffer ID");
		return -1;
	}

	db_print_vb("| Index buffer ID: %i\n", msg->ib_id);

	return 0;
}

int recv_tvr_mesh_bind_mat(
	int fd,
	struct tvr_msg_mesh_bind_mat *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: mesh ID (32-bit) */
	recv_status = recv(fd, &msg->mesh_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive mesh ID");
		return -1;
	}

	db_print_vb("| Mesh ID: %i\n", msg->mesh_id);

	/* Field: material ID (32-bit) */
	recv_status = recv(fd, &msg->mat_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive material ID");
		return -1;
	}

	db_print_vb("| Material ID: %i\n", msg->mat_id);

	return 0;
}

int recv_tvr_mesh_set_loc(
	int fd,
	struct tvr_msg_mesh_set_loc *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: mesh ID (32-bit) */
	recv_status = recv(fd, &msg->mesh_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive mesh ID");
		return -1;
	}

	db_print_vb("| Mesh ID: %i\n", msg->mesh_id);

	/* Field: X coordinate (32-bit float) */
	recv_status = recv(fd, &msg->x, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive X coordinate");
		return -1;
	}

	db_print_vb("| X coord: %f\n", msg->x);

	/* Field: Y coordinate (32-bit float) */
	recv_status = recv(fd, &msg->y, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Y coordinate");
		return -1;
	}

	db_print_vb("| Y coord: %f\n", msg->y);

	/* Field: Z coordinate (32-bit float) */
	recv_status = recv(fd, &msg->z, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Z coordinate");
		return -1;
	}

	db_print_vb("| Z coord: %f\n", msg->z);


	return 0;
}

int recv_tvr_mesh_set_rot(
	int fd,
	struct tvr_msg_mesh_set_rot *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: mesh ID (32-bit) */
	recv_status = recv(fd, &msg->mesh_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive mesh ID");
		return -1;
	}

	db_print_vb("| Mesh ID: %i\n", msg->mesh_id);

	/* Field: X coordinate (32-bit float) */
	recv_status = recv(fd, &msg->x, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive X coordinate");
		return -1;
	}

	db_print_vb("| X coord: %f\n", msg->x);

	/* Field: Y coordinate (32-bit float) */
	recv_status = recv(fd, &msg->y, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Y coordinate");
		return -1;
	}

	db_print_vb("| Y coord: %f\n", msg->y);

	/* Field: Z coordinate (32-bit float) */
	recv_status = recv(fd, &msg->z, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Z coordinate");
		return -1;
	}

	db_print_vb("| Z coord: %f\n", msg->z);


	return 0;
}

int recv_tvr_mesh_set_scale(
	int fd,
	struct tvr_msg_mesh_set_scale *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: mesh ID (32-bit) */
	recv_status = recv(fd, &msg->mesh_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive mesh ID");
		return -1;
	}

	db_print_vb("| Mesh ID: %i\n", msg->mesh_id);

	/* Field: X coordinate (32-bit float) */
	recv_status = recv(fd, &msg->x, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive X coordinate");
		return -1;
	}

	db_print_vb("| X coord: %f\n", msg->x);

	/* Field: Y coordinate (32-bit float) */
	recv_status = recv(fd, &msg->y, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Y coordinate");
		return -1;
	}

	db_print_vb("| Y coord: %f\n", msg->y);

	/* Field: Z coordinate (32-bit float) */
	recv_status = recv(fd, &msg->z, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Z coordinate");
		return -1;
	}

	db_print_vb("| Z coord: %f\n", msg->z);

	return 0;
}

int recv_tvr_mesh_destroy(
	int fd,
	struct tvr_msg_mesh_destroy *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);
	
	/* Field: mesh ID (32-bit) */
	recv_status = recv(fd, &msg->mesh_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive mesh ID");
		return -1;
	}

	db_print_vb("| Mesh ID: %i\n", msg->mesh_id);

	return 0;
}

/* texture requests */

int recv_tvr_texture_create(
	int fd,
	struct tvr_msg_texture_create *msg
)
{
	int recv_status;
	uint64_t tex_size;	
	uintptr_t tex_recv_offset = 0;

	db_print_vb("| Client ID: %i\n", fd);

	/* Field: texture ID (32-bit) */
	recv_status = recv(fd, &msg->tex_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive texture ID");
		return -1;
	}

	db_print_vb("| Texture ID: %i\n", msg->tex_id);

	/* Field: texture width (32-bit) */
	recv_status = recv(fd, &msg->width, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive texture width");
		return -1;
	}

	db_print_vb("| Texture width: %i\n", msg->width);

	/* Field: texture height (32-bit) */
	recv_status = recv(fd, &msg->height, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive texture height");
		return -1;
	}

	db_print_vb("| Texture height: %i\n", msg->height);

	/* Field: Number of texture channels (8-bit) */
	recv_status = recv(fd, &msg->channels, 1, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive texture channel count");
		return -1;
	}

	db_print_vb("| Texture channel count: %i\n", msg->channels);

	tex_size = msg->width * msg->height * msg->channels;

	db_print_vb("| Texture data size %i bytes\n", tex_size);

	/* Texture data */
	msg->data = malloc(tex_size);
	if (!msg->data)
	{
		perror("Failed to allocate space for texture");
		return -1;
	}

	while (tex_recv_offset < tex_size)
	{
		recv_status = recv(
			fd,
			msg->data + tex_recv_offset,
			tex_size - tex_recv_offset,
			0
		);

		if (recv_status <= 0)
		{
			perror("Failed to receive texture data");
			return -1;
		}

		tex_recv_offset += recv_status;
	}

	return 0;
}

int recv_tvr_texture_destroy(
	int fd,
	struct tvr_msg_texture_destroy *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: texture ID (32-bit) */
	recv_status = recv(fd, &msg->tex_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive texture ID");
		return -1;
	}

	db_print_vb("| Texture ID: %i\n", msg->tex_id);


	return 0;
}

/* material requests */

int recv_tvr_material_create(
	int fd,
	struct tvr_msg_material_create *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: material ID (32-bit) */
	recv_status = recv(fd, &msg->mat_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive material ID");
		return -1;
	}

	db_print_vb("| Material ID: %i\n", msg->mat_id);



	return 0;
}

int recv_tvr_material_bind_texture(
	int fd,
	struct tvr_msg_material_bind_texture *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: material ID (32-bit) */
	recv_status = recv(fd, &msg->mat_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive material ID");
		return -1;
	}

	db_print_vb("| Material ID: %i\n", msg->mat_id);

	/* Field: texture ID (32-bit) */
	recv_status = recv(fd, &msg->tex_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive texture ID");
		return -1;
	}

	db_print_vb("| Texture ID: %i\n", msg->tex_id);


	/* Field: render pass (8-bit) */
	recv_status = recv(fd, &msg->pass, 1, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive pass number");
		return -1;
	}

	db_print_vb("| Pass: %i\n", msg->pass);

	return 0;
}

int recv_tvr_material_destroy(
	int fd,
	struct tvr_msg_material_destroy *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: material ID (32-bit) */
	recv_status = recv(fd, &msg->mat_id, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive material ID");
		return -1;
	}

	db_print_vb("| Material ID: %i\n", msg->mat_id);

	return 0;
}

/* pov requests */

int recv_tvr_pov_set_loc(
	int fd,
	struct tvr_msg_pov_set_loc *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: X coordinate (32-bit float) */
	recv_status = recv(fd, &msg->x, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive X coordinate");
		return -1;
	}

	db_print_vb("| X coord: %f\n", msg->x);

	/* Field: Y coordinate (32-bit float) */
	recv_status = recv(fd, &msg->y, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Y coordinate");
		return -1;
	}

	db_print_vb("| Y coord: %f\n", msg->y);

	/* Field: Z coordinate (32-bit float) */
	recv_status = recv(fd, &msg->z, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Z coordinate");
		return -1;
	}

	db_print_vb("| Z coord: %f\n", msg->z);

	return 0;
}

int recv_tvr_pov_set_rot(
	int fd,
	struct tvr_msg_pov_set_rot *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: X coordinate (32-bit float) */
	recv_status = recv(fd, &msg->x, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive X coordinate");
		return -1;
	}

	db_print_vb("| X coord: %f\n", msg->x);

	/* Field: Y coordinate (32-bit float) */
	recv_status = recv(fd, &msg->y, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Y coordinate");
		return -1;
	}

	db_print_vb("| Y coord: %f\n", msg->y);

	/* Field: Z coordinate (32-bit float) */
	recv_status = recv(fd, &msg->z, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive Z coordinate");
		return -1;
	}

	db_print_vb("| Z coord: %f\n", msg->z);

	return 0;
}

int recv_tvr_pov_set_fov(
	int fd,
	struct tvr_msg_pov_set_fov *msg
)
{
	int recv_status;
		
	db_print_vb("| Client ID: %i\n", fd);

	/* Field: FOV (32-bit float) */
	recv_status = recv(fd, &msg->fov, 4, 0);

	if (recv_status <= 0)
	{
		perror("Failed to receive FOV");
		return -1;
	}

	db_print_vb("| FOV: %f\n", msg->fov);

	return 0;
}



