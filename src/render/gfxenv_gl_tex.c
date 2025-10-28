#include "gfxenv.h"
#include "opengl.h"
#include "../debug/print.h"
#include "../io/tvrender.h"
#include "../common/id.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libtvrender/tvr.h>

int gfxenv_create_texture_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_texture_create msg;
	struct gl_texture new_tex;
	
	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (create texture)\n");

	/* Receive incoming message */
	if (recv_tvr_texture_create(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Extend array */
	if (gls->texture_count >= gls->texture_lim)
	{
		gls->texture_lim *= 2;

		/* Reallocate buffer array */
		new_addr = realloc(gls->textures, gls->texture_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for textures");
			return -1;
		}

		gls->textures = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(gls->texture_ids, gls->texture_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for texture IDs");
			return -1;
		}

		gls->texture_ids = new_addr;
	}

	/* Initialise texture */

	/* p.1: Set up bound material array */
	new_tex.mat_bind_count = 0;
	new_tex.mat_bind_lim = 1;
	new_tex.bound_mat_ids = malloc(sizeof(int));
	new_tex.bound_passes = malloc(sizeof(int));

	if (!new_tex.bound_mat_ids)
	{
		perror("Failed to allocate array of bound material IDs");
	}

	if (!new_tex.bound_passes)
	{
		perror("Failed to allocate array of bound render passes");
	}

	/* p.2: Set up bound mesh array */

	new_tex.bound_mesh_count = 0;
	new_tex.bound_mesh_lim = 1;
	new_tex.bound_mesh_ids = malloc(sizeof(int));
	new_tex.bound_mesh_passes = malloc(sizeof(int));

	if (!new_tex.bound_mesh_ids)
	{
		perror("Failed to allocate array of bound mesh IDs");
	}

	if (!new_tex.bound_mesh_passes)
	{
		perror("Failed to allocate array of bound mesh render passes");
	}

	/* p.3: Tell OpenGL to create and bind a new texture */
	glGenTextures(1, &new_tex.tex);	
	glBindTexture(GL_TEXTURE_2D, new_tex.tex);

	/* Texture wrapping/filtering */
	glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_REPEAT
	);

	glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			GL_REPEAT
	);

	glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR
	);

	glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR
	);

	/* Make OpenGL load texture data */
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			msg.width,
			msg.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			msg.data
	);

    glGenerateMipmap(GL_TEXTURE_2D);

	/* Add texture to texture array */
	gls->texture_ids[gls->texture_count] = msg.tex_id;
	gls->textures[gls->texture_count] = new_tex;

	/* Change buffer count */
	++gls->texture_count;

	return 0;
}

int gfxenv_destroy_texture_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_texture_destroy msg;
	int idx;
	int i;
	struct gl_texture *tex;
	int mat_idx, mesh_idx;

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (destroy texture)\n");

	/* Receive incoming message */
	if (recv_tvr_texture_destroy(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of ID in ID lookup table */

	idx = search_id(
		gls->texture_ids,
		gls->texture_count,
		msg.tex_id
	);

	if (idx == -1)
	{
		db_print_vb("Failed to find texture.\n");
		return -1;
	}

	tex = &gls->textures[idx];

	/* Destroy texture */

	/* Destruction pt. 1 - unbind texture from materials */

	i = 0;
	while (i < tex->mat_bind_count)
	{
		mat_idx = search_id(
			gls->mat_ids,
			gls->mat_count,
			tex->bound_mat_ids[i]
		);

		if (mat_idx == -1)
		{
			db_print_vb("Failed to find material\n");
			return -1;
		}

		switch (tex->bound_passes[i])
		{
		case TVR_PASS_ALBEDO:
			gls->mats[mat_idx].has_albedo = 0;
			break;

		default:
			db_print_vb(
				"Invalid render pass number: %i\n",
				tex->bound_passes[i]
			);
			break;
		}

		++i;
	}

	free(tex->bound_mat_ids);
	free(tex->bound_passes);

	/* Destruction pt. 2 - unbind texture from meshes */

	i = 0;
	while (i < tex->bound_mesh_count)
	{
		mesh_idx = search_id(
			gls->mesh_ids,
			gls->mesh_count,
			tex->bound_mesh_ids[i]
		);

		switch (tex->bound_mesh_passes[i])
		{
		case TVR_PASS_ALBEDO:
			gls->meshes[mesh_idx].albedo = gfxenv->data.gl.error_tex;
			break;

		default:
			db_print_vb(
				"Invalid render pass number: %i\n",
				tex->bound_passes[i]
			);
			break;
		}


		++i;
	}

	free(tex->bound_mesh_ids);	
	free(tex->bound_mesh_passes);	

	/* Destruction pt.3 - delete the texture itself */
	glDeleteTextures(1, &tex->tex);
	
	/* Shorten array and remove deleted texture from array */

	--gls->texture_count;

	memcpy(
		&gls->textures[idx],
		&gls->textures[idx + 1],
		(gls->texture_count - idx) * sizeof(*gls->textures)
	);

	memcpy(
		&gls->texture_ids[idx],
		&gls->texture_ids[idx + 1],
		(gls->texture_count - idx) * sizeof(*gls->texture_ids)
	);

	if (gls->texture_count > gls->texture_lim)
	{
		gls->texture_lim /= 2;

		/* Reallocate buffer array */
		new_addr = realloc(gls->textures, gls->texture_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for textures");
			return -1;
		}

		gls->textures = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(gls->texture_ids, gls->texture_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for texture IDs");
			return -1;
		}

		gls->texture_ids = new_addr;
	}

	return 0;
}

