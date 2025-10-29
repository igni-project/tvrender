#include "gfxenv.h"
#include "opengl.h"
#include "../debug/print.h"
#include "../io/tvrender.h"
#include "../common/id.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libtvrender/tvr.h>

int gfxenv_create_material_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_material_create msg;
	struct gl_material new_mat = {0};
	
	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (create mat)\n");

	/* Receive incoming message */
	if (recv_tvr_material_create(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Extend array */
	if (gls->mat_count >= gls->mat_lim)
	{
		gls->mat_lim *= 2;

		/* Reallocate buffer array */
		new_addr = realloc(
				gls->mats, gls->mat_lim * sizeof(struct gl_material));

		if (!new_addr)
		{
			perror("could not allocate memory for mats");
			return -1;
		}

		gls->mats = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(
				gls->mat_ids, gls->mat_lim * sizeof(struct gl_material));

		if (!new_addr)
		{
			perror("could not allocate memory for mat IDs");
			return -1;
		}

		gls->mat_ids = new_addr;
	}

	/* Initialise material */

	/* p.1 start all textures out as the default error texture */
	new_mat.albedo_id = 0;
	new_mat.has_albedo = 0;

	/* Add material to material array */
	gls->mat_ids[gls->mat_count] = msg.mat_id;
	gls->mats[gls->mat_count] = new_mat;

	/* Change buffer count */
	++gls->mat_count;

	return 0;
}

int gfxenv_material_bind_texture_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_material_bind_texture msg;
	int mat_idx, tex_idx;
	struct gl_texture *tex;

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (bind texture to material)\n");

	/* Receive incoming message */
	if (recv_tvr_material_bind_texture(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of material ID in ID lookup table */

	mat_idx = search_id(
		gls->mat_ids,
		gls->mat_count,
		msg.mat_id
	);

	if (mat_idx == -1)
	{
		db_print_vb("Failed to find mat.\n");
		return -1;
	}

	/* Copy texture to material */

	tex_idx = search_id(
		gls->texture_ids,
		gls->texture_count,
		msg.tex_id
	);

	if (tex_idx == -1)
	{
		db_print_vb("Failed to find tex.\n");
		return -1;
	}

	tex = &gls->textures[tex_idx];

	switch (msg.pass)
	{
	case TVR_PASS_ALBEDO:
		gls->mats[mat_idx].has_albedo = 1;
		gls->mats[mat_idx].albedo_id = msg.tex_id;
		break;
	default:
		db_print_vb("invalid pass number (%i)\n", msg.pass);
		break;
	}

	/* Log binding in texture */

	if (tex->mat_bind_count >= tex->mat_bind_lim)
	{
		tex->mat_bind_lim *= 2;

		/* Reallocate bound material ID array */
		new_addr = realloc(tex->bound_mat_ids, tex->mat_bind_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for tex->mat bind IDs");
			return -1;
		}

		tex->bound_mat_ids = new_addr;

		/* Reallocate bound render pass array */
		new_addr = realloc(tex->bound_passes, tex->mat_bind_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for list of render passes");
			return -1;
		}

		tex->bound_passes = new_addr;
	}

	tex->bound_passes[tex->mat_bind_count] = msg.pass;
	tex->bound_mat_ids[tex->mat_bind_count] = msg.mat_id;
	++tex->mat_bind_count;

	return 0;
}

int gfxenv_destroy_material_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_material_destroy msg;
	int idx = 0;

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (destroy mat)\n");

	/* Receive incoming message */
	if (recv_tvr_material_destroy(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of material in ID lookup table */

	idx = search_id(
		gls->mat_ids,
		gls->mat_count,
		msg.mat_id
	);

	if (idx == -1)
	{
		db_print_vb("Failed to find mat.\n");
		return -1;
	}

	/* Shorten array */

	--gls->mat_count;

	memcpy(
		&gls->mats[idx],
		&gls->mats[idx + 1],
		(gls->mat_count - idx) * sizeof(*gls->mats)
	);

	memcpy(
		&gls->mat_ids[idx],
		&gls->mat_ids[idx + 1],
		(gls->mat_count - idx) * sizeof(*gls->mat_ids)
	);

	if (gls->mat_count > gls->mat_lim)
	{
		gls->mat_lim /= 2;

		/* Reallocate buffer array */
		new_addr = realloc(
				gls->mats, gls->mat_lim * sizeof(struct gl_material));

		if (!new_addr)
		{
			perror("could not allocate memory for mats");
			return -1;
		}

		gls->mats = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(
				gls->mat_ids, gls->mat_lim * sizeof(struct gl_material));

		if (!new_addr)
		{
			perror("could not allocate memory for mat IDs");
			return -1;
		}

		gls->mat_ids = new_addr;
	}

	return 0;
}

