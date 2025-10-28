/* gfxenv_gl_vb.c */
/* OpenGL graphical environment - Vertex Buffers */

#include "gfxenv.h"
#include "opengl.h"
#include "../debug/print.h"
#include "../io/tvrender.h"
#include "../common/id.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gfxenv_create_vertex_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_vertex_buffer_create msg;
	struct gl_buffer new_vb = {0};

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (create vertex buffer)\n");

	/* Receive incoming message */
	if (recv_tvr_vertex_buffer_create(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	printf("Vertex buffers @ %p\n", gls->vertex_buffers);

	/* Extend array */
	if (gls->vertex_buffer_count >= gls->vertex_buffer_lim)
	{
		gls->vertex_buffer_lim *= 2;

		/* Reallocate buffer array */
		new_addr = realloc(gls->vertex_buffers, gls->vertex_buffer_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffers");
			return -1;
		}

		gls->vertex_buffers = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(gls->vertex_buffer_ids, gls->vertex_buffer_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffer IDs");
			return -1;
		}

		gls->vertex_buffer_ids = new_addr;
	}

	/* Create array of bound meshes	*/
	new_vb.bound_mesh_count = 0;
	new_vb.bound_mesh_lim = 1;
	new_vb.bound_mesh_ids = malloc(sizeof(int));

	if (!new_vb.bound_mesh_ids)
	{
		perror("Failed to allocate array of bound mesh IDs");
		return -1;
	}

	/* Init new buffer on the GL side */
	glGenBuffers(1, &new_vb.buf);

	if (new_vb.buf == GL_INVALID_VALUE)
	{
		printf("Failed to create vertex buffer.\n");
		return -1;
	}

	glBindBuffer(GL_ARRAY_BUFFER, new_vb.buf);
	glBufferData(GL_ARRAY_BUFFER, msg.size, msg.data, GL_DYNAMIC_DRAW);

	/* Unbind buffer to keep behaviour predictable */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* Add buffer to list */
	gls->vertex_buffers[gls->vertex_buffer_count] = new_vb;
	gls->vertex_buffer_ids[gls->vertex_buffer_count] = msg.vb_id;

	/* Change buffer count */
	++gls->vertex_buffer_count;

	return 0;
}

int gfxenv_destroy_vertex_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_vertex_buffer_destroy msg;
	int idx = 0;
	struct gl_buffer *vbo;
	int mesh_idx;
	int i;

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (destroy vertex buffer)\n");

	/* Receive incoming message */
	if (recv_tvr_vertex_buffer_destroy(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of ID in ID lookup table */

	idx = search_id(
		gls->vertex_buffer_ids,
		gls->vertex_buffer_count,
		msg.vb_id
	);

	if (idx == -1)
	{
		db_print_vb("Failed to find vertex buffer.\n");
		return -1;
	}

	vbo = &gls->vertex_buffers[idx];

	/* Remove all references to buffer */

	i = vbo->bound_mesh_count;
	while (i)
	{
		--i;

		mesh_idx = search_id(
			gls->mesh_ids,
			gls->mesh_count,
			vbo->bound_mesh_ids[i]
		);

		if (mesh_idx == -1)
		{
			db_print_vb("Failed to find bound mesh. skipping.\n");
			continue;
		}

		if (gfxenv_remove_mesh_manual_gl(gls, mesh_idx) == -1)
		{
			db_print_vb("Failed to remove bound mesh. skipping.\n");
			continue;
		}
	}

	free(vbo->bound_mesh_ids);

	/* Destroy buffer */
	
	glDeleteBuffers(1, &vbo->buf);

	/* Shorten array and remove deleted entry */

	--gls->vertex_buffer_count;

	memcpy(
		&gls->vertex_buffers[idx],
		&gls->vertex_buffers[idx + 1],
		(gls->vertex_buffer_count - idx) * sizeof(*gls->vertex_buffers)
	);

	memcpy(
		&gls->vertex_buffer_ids[idx],
		&gls->vertex_buffer_ids[idx + 1],
		(gls->vertex_buffer_count - idx) * sizeof(*gls->vertex_buffer_ids)
	);

	if (gls->vertex_buffer_count > gls->vertex_buffer_lim)
	{
		gls->vertex_buffer_lim /= 2;

		/* Reallocate buffer array */
		new_addr = realloc(gls->vertex_buffers, gls->vertex_buffer_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffers");
			return -1;
		}

		gls->vertex_buffers = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(gls->vertex_buffer_ids, gls->vertex_buffer_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffer IDs");
			return -1;
		}

		gls->vertex_buffer_ids = new_addr;
	}

	return 0;
}

