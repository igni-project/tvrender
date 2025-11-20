#include "gfxenv.h"
#include "opengl.h"
#include "../debug/print.h"
#include "../io/tvrender.h"
#include "../common/id.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gfxenv_set_pov_loc_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	struct tvr_msg_pov_set_loc msg;

	if (recv_tvr_pov_set_loc(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	db_print_vb("\033[1mNew request\033[0m (set POV location)\n");

	/* Calculate transformation */
	vec3 target = {0.0f};

	gfxenv->data.gl.pov.loc[0] = msg.x;
	gfxenv->data.gl.pov.loc[1] = msg.y;
	gfxenv->data.gl.pov.loc[2] = msg.z;
	glm_vec3_add(
		gfxenv->data.gl.pov.loc,
		gfxenv->data.gl.pov.fwd,
		target
	);

	glm_lookat(
		gfxenv->data.gl.pov.loc,
		target,
		gfxenv->data.gl.pov.up,
		gfxenv->data.gl.pov.view
	);

	/* Update UBO */
	glUniformMatrix4fv(
		gfxenv->data.gl.view_ubo,
		1,
		GL_FALSE,
		(GLfloat*)&gfxenv->data.gl.pov.view
	);

	return 0;
}

int gfxenv_set_pov_rot_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	struct tvr_msg_pov_set_rot msg;
	static vec3 x_axis = {1, 0, 0};
	static vec3 y_axis = {0, 1, 0};
	static vec3 z_axis = {0, 0, 1};

	if (recv_tvr_pov_set_rot(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	db_print_vb("\033[1mNew request\033[0m (set POV rotation)\n");

	/* Calculate transformation */
	vec3 target = {0.0f};

	gfxenv->data.gl.pov.fwd[0] = 0;
	gfxenv->data.gl.pov.fwd[1] = 0;
	gfxenv->data.gl.pov.fwd[2] = -1;
	glm_vec3_rotate(gfxenv->data.gl.pov.fwd, msg.x, x_axis);
	glm_vec3_rotate(gfxenv->data.gl.pov.fwd, msg.y, y_axis);
	glm_vec3_rotate(gfxenv->data.gl.pov.fwd, msg.z, z_axis);

	gfxenv->data.gl.pov.up[0] = 0;
	gfxenv->data.gl.pov.up[1] = 1;
	gfxenv->data.gl.pov.up[2] = 0;
	glm_vec3_rotate(gfxenv->data.gl.pov.up, msg.x, x_axis);
	glm_vec3_rotate(gfxenv->data.gl.pov.up, msg.y, y_axis);
	glm_vec3_rotate(gfxenv->data.gl.pov.up, msg.z, z_axis);

	glm_vec3_add(
		gfxenv->data.gl.pov.loc,
		gfxenv->data.gl.pov.fwd,
		target
	);

	glm_lookat(
		gfxenv->data.gl.pov.loc,
		target,
		gfxenv->data.gl.pov.up,
		gfxenv->data.gl.pov.view
	);

	/* Update UBO */
	glUniformMatrix4fv(
		gfxenv->data.gl.view_ubo,
		1,
		GL_FALSE,
		(GLfloat*)&gfxenv->data.gl.pov.view
	);

	return 0;
}

int gfxenv_pov_look_at_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	struct tvr_msg_pov_look_at msg;
	vec3 target = {0};

	if (recv_tvr_pov_look_at(
			gfxenv->scenes[scene].fd,
			&msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	db_print_vb("\033[1mNew request\033[0m (POV: look at point)\n");

	target[0]= msg.x;
	target[1] = msg.y;
	target[2] = msg.z;
	
	glm_lookat(
		gfxenv->data.gl.pov.loc,
		target,
		gfxenv->data.gl.pov.up,
		gfxenv->data.gl.pov.view
	);
	
	/* Update UBO */
	glUniformMatrix4fv(
		gfxenv->data.gl.view_ubo,
		1,
		GL_FALSE,
		(GLfloat*)&gfxenv->data.gl.pov.view
	);

	return 0;
}

int gfxenv_set_pov_fov_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	struct tvr_msg_pov_set_fov msg;
	static vec3 x_axis = {1, 0, 0};
	static vec3 y_axis = {0, 1, 0};
	static vec3 z_axis = {0, 0, 1};

	db_print_vb("\033[1mNew request\033[0m (set POV FOV)\n");

	if (recv_tvr_pov_set_fov(
			gfxenv->scenes[scene].fd, &msg) == -1)
	{
		db_print_vb("failed to recieve message.\n");
		return -1;
	}

	glm_mat4_identity(gfxenv->data.gl.pov.proj);
	glm_perspective( 
		msg.fov, /* deg to radian conversion */ 
		800.0f / 600.0f, 
		0.1f, 
		100.0f,
		gfxenv->data.gl.pov.proj
	);

	vec3 up = {0, 1.0f, 0};
	vec3 fwd = {0, 0, -1.0f};


	/* Update UBO */
	glUniformMatrix4fv(
		gfxenv->data.gl.proj_ubo,
		1,
		GL_FALSE,
		(GLfloat*)&gfxenv->data.gl.pov.proj
	);

	return 0;
}

