#include "gfxenv.h"
#include "../debug/print.h"
#include "../io/tvrender.h"

#include <libtvrender/tvr.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

int (*p_gfxenv_render)(struct gfxenv *, GLFWwindow*) = &gfxenv_render_na;
void (*p_destroy_gfxenv)(struct gfxenv) = &destroy_gfxenv_na;
int (*p_gfxenv_new_client)(struct gfxenv *, int) = &gfxenv_new_client_na;
int (*p_gfxenv_exec)(struct gfxenv *, unsigned int) = &gfxenv_exec_na;

int create_gfxenv(struct gfxenv *gfxenv, GLFWwindow *window)
{
	gfxenv->scenes = malloc(sizeof(struct gfxenv_scene));
	gfxenv->scene_count = 0;
	gfxenv->scene_limit = 1;

	if (!gfxenv->scenes)
	{
		db_print_vb("Could not allocate memory for scenes!\n");
		return -1;
	}

	/* As of now, GL is the sole graphics library supported */
	if (create_gfxenv_gl(gfxenv, window) == -1)
	{
		return -1;
	}

	return 0;
}

int gfxenv_render_na(struct gfxenv *gfxenv, GLFWwindow *window)
{
	db_print_vb("Headless mode unsupported! quitting.\n");
	exit(EXIT_FAILURE);
	return -1;
}

void destroy_gfxenv_na(struct gfxenv gfxenv)
{
	free(gfxenv.scenes);
}

int gfxenv_new_client_na(struct gfxenv *gfxenv, int fd)
{
	/* The reallocated address is audited before use */
	void *new_addr;

	if (gfxenv->scene_count >= gfxenv->scene_limit)
	{
		gfxenv->scene_limit *= 2;

		/* Reallocate socket descriptor array */
		new_addr = realloc(gfxenv->scenes, gfxenv->scene_limit);
		if (!new_addr)
		{
			perror("add_new_client() failed: could not allocate memory");
			return -1;
		}

		gfxenv->scenes = new_addr;

	}

	gfxenv->scenes[gfxenv->scene_count].fd = fd;
	gfxenv->scenes[gfxenv->scene_count].tvr_ver = 0;


	++gfxenv->scene_count;

	db_print_vb("\033[1mNew client\033[0m\n");
	db_print_vb("| Client ID: %i\n", fd);

	return 0;
}

int gfxenv_del_scene_na(struct gfxenv *gfxenv, unsigned int scene)
{
	void *new_addr;

	if (scene > gfxenv->scene_count)
	{
		db_print_vb("Scene index out of range! (%i)\n", scene);
		return -1;
	}

	close(gfxenv->scenes[scene].fd);

	db_print_vb("| Client ID: %i\n", gfxenv->scenes[scene].fd);
	
	--gfxenv->scene_count;

	memcpy(
		&gfxenv->scenes[scene],
		&gfxenv->scenes[scene + 1],
		(gfxenv->scene_count - scene) * sizeof(struct gfxenv_scene)
	);

	if (gfxenv->scene_count < gfxenv->scene_limit / 2)
	{
		gfxenv->scene_limit /= 2;

		/* Reallocate socket descriptor array */
		new_addr = realloc(gfxenv->scenes, gfxenv->scene_limit);
		if (!new_addr)
		{
			perror("realloc() in gfxenv_del_scene_na() failed:");
			return -1;
		}
		gfxenv->scenes = (struct gfxenv_scene *)new_addr;
	}
	
	return 0;
}   

int gfxenv_exec_na(struct gfxenv *gfxenv, unsigned int scene)
{
	db_print_vb("Headless mode unsupported! quitting.\n");
	exit(EXIT_FAILURE);
	return -1;
}

int gfxenv_set_tvr_version(struct gfxenv *gfxenv, unsigned int scene)
{
	struct tvr_msg_set_version msg;

	db_print_vb("\033[1mNew request\033[0m (set protocol version)\n");

	if (recv_tvr_set_version(
			gfxenv->scenes[scene].fd,
			&msg) == -1)
	{ 
		db_print_vb("failed to recieve message.\n"); 
		return -1; 
	} 
 
	gfxenv->scenes[scene].tvr_ver = msg.version; 
 
	return 0;
}

