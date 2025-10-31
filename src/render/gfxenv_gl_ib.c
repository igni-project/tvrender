/* gfxenv_gl_vb.c */
/* OpenGL graphical environment - Index Buffers */

#include "gfxenv.h"
#include "opengl.h"
#include "../debug/print.h"
#include "../io/tvrender.h"
#include "../common/id.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gfxenv_create_index_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_index_buffer_create msg;
	struct gl_buffer new_ib = {0};

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (create index buffer)\n");

	/* Receive incoming message */
	if (recv_tvr_index_buffer_create(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Extend array */
	if (gls->index_buffer_count >= gls->index_buffer_lim)
	{
		gls->index_buffer_lim *= 2;

		/* Reallocate buffer array */
		new_addr = realloc(
			gls->index_buffers,
			gls->index_buffer_lim * sizeof(struct gl_buffer)
		);
		if (!new_addr)
		{
			perror("could not allocate memory for index buffer array");
			return -1;
		}

		gls->index_buffers = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(
			gls->index_buffer_ids,
			gls->index_buffer_lim * sizeof(int)
		);
		if (!new_addr)
		{
			perror("could not allocate memory for index buffer IDs");
			return -1;
		}

		gls->index_buffer_ids = new_addr;
	}

	/* Create array of bound meshes	*/
	new_ib.bound_mesh_count = 0;
	new_ib.bound_mesh_lim = 1;
	new_ib.bound_mesh_ids = malloc(sizeof(int));

	if (!new_ib.bound_mesh_ids)
	{
		perror("Failed to allocate array of bound mesh IDs");
		return -1;
	}

	/* Init new buffer on the GL side */
	glGenBuffers(1, &new_ib.buf);

	if (new_ib.buf == GL_INVALID_VALUE)
	{
		printf("Failed to create index buffer.\n");
		return -1;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_ib.buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, msg.size, msg.data, GL_STATIC_DRAW);

	/* Unbind buffer to keep behaviour predictable */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* Add buffer to list */
	gls->index_buffers[gls->index_buffer_count] = new_ib;
	gls->index_buffer_ids[gls->index_buffer_count] = msg.ib_id;

	/* Change buffer count */
	++gls->index_buffer_count;

	return 0;
}

int gfxenv_destroy_index_buffer_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_index_buffer_destroy msg;
	int idx = 0;
	int mesh_idx;
	int i;
	struct gl_buffer *ibo;

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (destroy index buffer)\n");

	/* Receive incoming message */
	if (recv_tvr_index_buffer_destroy(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of mesh ID in ID lookup table */

	idx = search_id(
		gls->index_buffer_ids,
		gls->index_buffer_count,
		msg.ib_id
	);

	if (idx == -1)
	{
		db_print_vb("Failed to find index buffer.\n");
		return -1;
	}

	ibo = &gls->index_buffers[idx];

	/* Remove all references to buffer */

	i = ibo->bound_mesh_count;
	while (i)
	{
		--i;

		mesh_idx = search_id(
			gls->mesh_ids,
			gls->mesh_count,
			ibo->bound_mesh_ids[i]
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

	/* a memory leak is better than an error i suppose. */
	free(ibo->bound_mesh_ids);

	/* Destroy buffer */
	
	glDeleteBuffers(1, &ibo->buf);

	/* Shorten array and remove deleted entry */

	--gls->index_buffer_count;

	memcpy(
		&gls->index_buffers[idx],
		&gls->index_buffers[idx + 1],
		(gls->index_buffer_count - idx) * sizeof(*gls->index_buffers)
	);

	memcpy(
		&gls->index_buffer_ids[idx],
		&gls->index_buffer_ids[idx + 1],
		(gls->index_buffer_count - idx) * sizeof(*gls->index_buffer_ids)
	);

	if (gls->index_buffer_count > gls->index_buffer_lim)
	{
		gls->index_buffer_lim /= 2;

		/* Reallocate buffer array */
		new_addr = realloc(
			gls->index_buffers,
			gls->index_buffer_lim * sizeof(struct gl_buffer)
		);
		if (!new_addr)
		{
			perror("could not allocate memory for index buffers");
			return -1;
		}

		gls->index_buffers = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(
			gls->index_buffer_ids,
			gls->index_buffer_lim * sizeof(int)
		);
		if (!new_addr)
		{
			perror("could not allocate memory for index buffer IDs");
			return -1;
		}

		gls->index_buffer_ids = new_addr;
	}

	return 0;
}

