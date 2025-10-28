#include "gfxenv.h"
#include "opengl.h"
#include "../debug/print.h"
#include "../io/tvrender.h"
#include "../common/id.h"

#include <libtvrender/tvr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gfxenv_create_mesh_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_mesh_create msg;
	struct gl_mesh new_mesh = {0};
	int ib_idx, vb_idx;
	struct gl_buffer *vb, *ib;
	vec3 loc, rot, tl;

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (create mesh)\n");

	/* Receive incoming message */
	if (recv_tvr_mesh_create(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Extend array */
	if (gls->mesh_count >= gls->mesh_lim)
	{
		gls->mesh_lim *= 2;

		/* Reallocate buffer array */
		new_addr = realloc(gls->meshes, gls->mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for meshes");
			return -1;
		}

		gls->meshes = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(gls->mesh_ids, gls->mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for mesh IDs");
			return -1;
		}

		gls->mesh_ids = new_addr;
	}

	/* Search for index of index buffer ID in ID lookup table */
	ib_idx = search_id(
		gls->index_buffer_ids,
		gls->index_buffer_count,
		msg.ib_id
	);

	if (ib_idx == -1)
	{
		db_print_vb("Failed to find index buffer.\n");
		return -1;
	}

	/* Search for index of vertex buffer ID in ID lookup table */
	vb_idx = search_id(
		gls->vertex_buffer_ids,
		gls->vertex_buffer_count,
		msg.vb_id
	);

	if (vb_idx == -1)
	{
		db_print_vb("Failed to find vertex buffer.\n");
		return -1;
	}

	/* Create new mesh */
	glGenVertexArrays(1, &new_mesh.vertex_array);

	/* Modifying the vertex array */
	glBindVertexArray(new_mesh.vertex_array);

	/* Bind vertex and index buffers */
	vb = &gls->vertex_buffers[vb_idx];
	glBindBuffer(GL_ARRAY_BUFFER, vb->buf);
	
	ib = &gls->index_buffers[ib_idx];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->buf);

	/* Log vertex and index buffer bindings */

	/* p.1: vertex buffer bindings */

	if (vb->bound_mesh_count >= vb->bound_mesh_lim)
	{
		vb->bound_mesh_lim *= 2;

		/* Reallocate buffer array */
		new_addr = realloc(vb->bound_mesh_ids, vb->bound_mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vb bound mesh IDs");
			return -1;
		}

		vb->bound_mesh_ids = new_addr;
	}

	vb->bound_mesh_ids[vb->bound_mesh_count] = msg.mesh_id;
	++vb->bound_mesh_count;

	/* p.2: index buffer bindings */

	if (ib->bound_mesh_count >= ib->bound_mesh_lim)
	{
		ib->bound_mesh_lim *= 2;

		/* Reallocate buffer array */
		new_addr = realloc(ib->bound_mesh_ids, ib->bound_mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for ib bound mesh IDs");
			return -1;
		}

		ib->bound_mesh_ids = new_addr;
	}

	ib->bound_mesh_ids[ib->bound_mesh_count] = msg.mesh_id;
	++ib->bound_mesh_count;

	/* Vertex Attributes */
	/* attrib. 0: position */
	glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(struct vertex),
			(void*)0
	);
	glEnableVertexAttribArray(0);  

	/* attrib. 1: normals */
	glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(struct vertex), 
			(void*)(sizeof(float) * 3)
	);
	glEnableVertexAttribArray(1);  

	/* attrib. 2: texture coordinates */
	glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(struct vertex),
			(void*)(sizeof(float) * 6)
	);
	glEnableVertexAttribArray(2);  

	/* Done modifying vertex array */
	glBindVertexArray(new_mesh.vertex_array);

	/* Init mesh material */
	new_mesh.albedo = gfxenv->data.gl.error_tex;

	/* Add mesh to mesh array */
	gls->mesh_ids[gls->mesh_count] = msg.mesh_id;
	gls->meshes[gls->mesh_count] = new_mesh;

	/* Change mesh count */
	++gls->mesh_count;

	return 0;
}

int gfxenv_mesh_bind_mat_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_mesh_bind_mat msg;
	int mesh_idx, mat_idx;
	struct gl_mesh *mesh;
	struct gl_material *mat;

	gls = &gfxenv->data.gl.scenes[scene];
	
	db_print_vb("\033[1mNew request\033[0m (bind material to mesh)\n");
	
	/* Receive incoming message */
	if (recv_tvr_mesh_bind_mat(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of mesh ID in ID lookup table */
	mesh_idx = search_id(
		gls->mesh_ids,
		gls->mesh_count,
		msg.mesh_id
	);

	if (mesh_idx == -1)
	{
		db_print_vb("Failed to find mesh.\n");
		return -1;
	}

	mesh = &gls->meshes[mesh_idx];

	/* Search for index of mat ID in ID lookup table */
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

	mat = &gls->mats[mat_idx];

	/* Copy material to mesh */
	if (gfxenv_mesh_bind_mat_manual_gl(gls, mesh_idx, mat_idx) == -1)
	{
		printf("Failed to copy material to mesh\n");
		return -1;
	}

	return 0;
}

int gfxenv_mesh_bind_mat_manual_gl(
	struct gl_scene *gls,
	int mesh_idx,
	int mat_idx
)
{
	void *new_addr;
	int albedo_idx;

	struct gl_mesh *mesh;
	struct gl_material *mat;
	struct gl_texture *alb;

	mesh = &gls->meshes[mesh_idx];
	mat = &gls->mats[mat_idx];

	/* Search for index of albedo texture ID in ID lookup table */
	albedo_idx = search_id(
		gls->texture_ids,
		gls->texture_count,
		mat->albedo_id
	);

	if (mat->has_albedo)
	{
		if (albedo_idx == -1)
		{
			db_print_vb("Could not find albedo texture\n");
			return -1;
		}

		mesh->albedo = gls->textures[albedo_idx].tex;
		alb = &gls->textures[albedo_idx];

		/* Log binding in texture */

		if (alb->bound_mesh_count >= alb->bound_mesh_lim)
		{
			alb->bound_mesh_lim *= 2;

			/* Reallocate bound mesh ID array */
			new_addr = realloc(alb->bound_mesh_ids, alb->bound_mesh_lim);
			if (!new_addr)
			{
				perror("could not allocate memory for tex->mesh bind IDs");
				return -1;
			}

			alb->bound_mesh_ids = new_addr;
		}

		alb->bound_mesh_passes[alb->bound_mesh_count] = TVR_PASS_ALBEDO;
		alb->bound_mesh_ids[alb->bound_mesh_count] = gls->mesh_ids[mesh_idx];

		++alb->bound_mesh_count;
	}

	return 0;
}

int gfxenv_mesh_set_loc_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	struct gl_scene *gls;
	struct tvr_msg_mesh_set_loc msg;
	int mesh_idx;
	struct gl_mesh *mesh;

	gls = &gfxenv->data.gl.scenes[scene];
	
	db_print_vb("\033[1mNew request\033[0m (set mesh location)\n");

	/* Receive incoming message */
	if (recv_tvr_mesh_set_loc(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}
	
	/* Search for index of mesh ID in ID lookup table */
	mesh_idx = search_id(
		gls->mesh_ids,
		gls->mesh_count,
		msg.mesh_id
	);

	if (mesh_idx == -1)
	{
		db_print_vb("Failed to find mesh.\n");
		return -1;
	}

	/* Translate mesh */
	
	mesh = &gls->meshes[mesh_idx];
	mesh->loc[0] = msg.x;
	mesh->loc[1] = msg.y;
	mesh->loc[2] = msg.z;

	gfxenv_mesh_apply_tf_gl(mesh);

	return 0;
}

int gfxenv_mesh_set_rot_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	struct gl_scene *gls;
	struct tvr_msg_mesh_set_rot msg;
	int mesh_idx;
	struct gl_mesh *mesh;
	float rot[4][4] = {0};
	float scale[4][4] = {0};

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (set mesh rotation)\n");

	/* Receive incoming message */
	if (recv_tvr_mesh_set_rot(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of mesh ID in ID lookup table */
	mesh_idx = search_id(
		gls->mesh_ids,
		gls->mesh_count,
		msg.mesh_id
	);

	if (mesh_idx == -1)
	{
		db_print_vb("Failed to find mesh.\n");
		return -1;
	}

	/* Rotate mesh */
	mesh = &gls->meshes[mesh_idx];
	mesh->rot[0] = msg.x;
	mesh->rot[1] = msg.y;
	mesh->rot[2] = msg.z;

	gfxenv_mesh_apply_tf_gl(mesh);

	return 0;
}

int gfxenv_mesh_set_scale_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	struct gl_scene *gls;
	struct tvr_msg_mesh_set_scale msg;
	int mesh_idx;
	struct gl_mesh *mesh;
	float rot[4] = {0};

	gls = &gfxenv->data.gl.scenes[scene];
	
	db_print_vb("\033[1mNew request\033[0m (set mesh scale)\n");

	/* Receive incoming message */
	if (recv_tvr_mesh_set_scale(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}
	
	/* Search for index of mesh ID in ID lookup table */
	mesh_idx = search_id(
		gls->mesh_ids,
		gls->mesh_count,
		msg.mesh_id
	);

	if (mesh_idx == -1)
	{
		db_print_vb("Failed to find mesh.\n");
		return -1;
	}

	/* Scale mesh */
	mesh = &gls->meshes[mesh_idx];
	mesh->scale[0] = msg.x;
	mesh->scale[1] = msg.y;
	mesh->scale[2] = msg.z;

	gfxenv_mesh_apply_tf_gl(mesh);

	return 0;
}

int gfxenv_destroy_mesh_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;
	struct gl_scene *gls;
	struct tvr_msg_mesh_destroy msg;
	int idx = 0;

	gls = &gfxenv->data.gl.scenes[scene];

	db_print_vb("\033[1mNew request\033[0m (destroy mesh)\n");

	/* Receive incoming message */
	if (recv_tvr_mesh_destroy(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	/* Search for index of ID in ID lookup table */

	idx = search_id(
		gls->mesh_ids,
		gls->mesh_count,
		msg.mesh_id
	);

	if (idx == -1)
	{
		db_print_vb("Failed to find mesh.\n");
		return -1;
	}

	/* Destroy buffer */
	
	glDeleteVertexArrays(1, &gls->meshes[idx].vertex_array);

	/* Shorten array */

	--gls->mesh_count;

	memcpy(
		&gls->meshes[idx],
		&gls->meshes[idx + 1],
		(gls->mesh_count - idx) * sizeof(*gls->meshes)
	);

	memcpy(
		&gls->mesh_ids[idx],
		&gls->mesh_ids[idx + 1],
		(gls->mesh_count - idx) * sizeof(*gls->mesh_ids)
	);

	if (gls->mesh_count > gls->mesh_lim)
	{
		gls->mesh_lim /= 2;

		/* Reallocate buffer array */
		new_addr = realloc(gls->meshes, gls->mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffers");
			return -1;
		}

		gls->meshes = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(gls->mesh_ids, gls->mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffer IDs");
			return -1;
		}

		gls->mesh_ids = new_addr;
	}

	return 0;
}

int gfxenv_remove_mesh_manual_gl(
	struct gl_scene *gls,
	int idx
)
{
	void *new_addr;

	db_print_vb("\033[1mRemoving mesh\033[0m %i\n", idx);

	/* Destroy vertex array object */
	
	glDeleteVertexArrays(1, &gls->meshes[idx].vertex_array);

	/* Shorten array */

	--gls->mesh_count;

	memcpy(
		&gls->meshes[idx],
		&gls->meshes[idx + 1],
		(gls->mesh_count - idx) * sizeof(*gls->meshes)
	);

	memcpy(
		&gls->mesh_ids[idx],
		&gls->mesh_ids[idx + 1],
		(gls->mesh_count - idx) * sizeof(*gls->mesh_ids)
	);

	if (gls->mesh_count > gls->mesh_lim)
	{
		gls->mesh_lim /= 2;

		/* Reallocate buffer array */
		new_addr = realloc(gls->meshes, gls->mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffers");
			return -1;
		}

		gls->meshes = new_addr;

		/* Reallocate ID array */
		new_addr = realloc(gls->mesh_ids, gls->mesh_lim);
		if (!new_addr)
		{
			perror("could not allocate memory for vertex buffer IDs");
			return -1;
		}

		gls->mesh_ids = new_addr;
	}

	return 0;
}

void gfxenv_mesh_apply_tf_gl(struct gl_mesh *mesh)
{
	static vec3 x_axis = {1, 0, 0};
	static vec3 y_axis = {0, 1, 0};
	static vec3 z_axis = {0, 0, 1};

	glm_mat4_identity(mesh->tf);
	glm_translate(mesh->tf, mesh->loc);
	glm_rotate(mesh->tf, mesh->rot[0], x_axis);
	glm_rotate(mesh->tf, mesh->rot[1], y_axis);
	glm_rotate(mesh->tf, mesh->rot[2], z_axis);
	glm_scale(mesh->tf, mesh->scale);
}

