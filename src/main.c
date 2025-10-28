#include "main.h"
#include "debug/print.h"
#include "cmd/cmdline.h"
#include "render/gfxenv.h"
#include "render/rflag.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define FPS_CAP 60

int main(int argc, char **argv, char **envp)
{
	/* Server socket */
	struct sockaddr_un sv_addr = {0};
	int srv_fd;

	/* Variables for accept() */
	struct sockaddr_un tmp_addr = {0};
	socklen_t tmp_addr_sz = 0;
	int new_fd;

	/* Set of all sockets */
	fd_set read_fds;
	int max_fd;

	/* keep main loop going */
	char shouldClose = 0;

	/* Socket activity */
	int activity;

	/* Window */
	GLFWwindow *window = 0;

	/* The graphical environment */
	struct gfxenv gfxenv = {0};

	/* In windowed mode, the display server refreshes at a constant rate
	 * to detect window closing and resizing. */
	static struct timeval idleFrameTime = {0};

	/* This variable records the time the last frame was rendered. */
	clock_t frameTime = 0;

	/* Counter */
	int i;

	/* Command-line arguments */
	struct gengetopt_args_info args_info = {};

	/* Parse command line arguments */
	if (cmdline_parser(argc, argv, &args_info))
	{
		printf("Failed to parse command line arguments\n");
	}

	srv_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (srv_fd == -1)
	{
		perror("Failed to create server socket");
		return -1;
	}

	sv_addr.sun_family = AF_UNIX;
	strncpy(sv_addr.sun_path, args_info.domain_arg, sizeof(sv_addr.sun_path));
	if (bind(srv_fd, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) == -1)
	{
		perror("Failed to bind server socket");
		return -1;
	}

	if (listen(srv_fd, 10) == -1)
	{
		perror("Failed to open server socket for listening");
		return -1;
	}

	/* Start child processes */
	i = 0;
	while (i < args_info.inputs_num)
	{
		if (args_info.verbose_flag)
		{
			printf("Executing %s\n", args_info.inputs[i]);
		}

		if (!fork()) break;

		if (execve(args_info.inputs[i], 0, envp) == -1) {
			printf("Failed to execute ");
			perror(args_info.inputs[i]);
			exit(EXIT_FAILURE);
		}

		++i;
	}

	/* Configure debug messages */
	if (args_info.verbose_flag)
	{
		db_verbose_enable();
	}

	/* Create graphical environment */
	if (glfwInit() == GLFW_FALSE)
	{
		printf("Failed to initialise GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(800, 600, "TVrender", 0, 0);
	if (!window)
	{
		printf("Failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (create_gfxenv(&gfxenv, window) == -1)
	{
		return -1;
	}

	/* The file descriptor of the server socket is higher than all zero of the
	 * other active sockets. */
	max_fd = srv_fd;

	/* MAIN LOOP */
	while (!shouldClose)
	{
		/* This code must occur every frame. */
		FD_ZERO(&read_fds);
		FD_SET(srv_fd, &read_fds);

		i = 0;
		while (i < gfxenv.scene_count)
		{
			FD_SET(gfxenv.scenes[i].fd, &read_fds);

			if (gfxenv.scenes[i].fd > max_fd)
			{
				max_fd = gfxenv.scenes[i].fd;
			}

			++i;
		}

		/* select() may edit the timespec so it has to be reset all the time */
		idleFrameTime.tv_sec = 0;
		idleFrameTime.tv_usec = 1.0 / FPS_CAP * 1000.0;

		/* Wait until a socket is active */
		activity = select(max_fd + 1, &read_fds, 0, 0, &idleFrameTime);

		if (activity == -1 && errno != EINTR)
		{
			perror("Socket select failed");
		}

		/* Activity on the server socket means a new connection */
		if (FD_ISSET(srv_fd, &read_fds))
		{
			new_fd = accept(srv_fd, (struct sockaddr*)&tmp_addr, &tmp_addr_sz);
			if (new_fd == -1)
			{
				perror("Failed to accept client");
			}
			else
			{
				p_gfxenv_new_client(&gfxenv, new_fd);
			}
		}

		/* Execute incoming SUP commands */
		i = gfxenv.scene_count - 1;
		while (i != -1)
		{
			if (FD_ISSET(gfxenv.scenes[i].fd, &read_fds))
			{
				p_gfxenv_exec(&gfxenv, i);
			}

			--i;
		}

		/* This sets a maximum framerate and allows multiple
		 * commands to process between frames. */
		if (clock() - frameTime > CLOCKS_PER_SEC / FPS_CAP) {
			frameTime = clock();

			/* Render scene and update window */
			p_gfxenv_render(&gfxenv, window);
		}

		glfwPollEvents();
		shouldClose = glfwWindowShouldClose(window);
	}

	/* Clean up before exit */
	p_destroy_gfxenv(gfxenv);
	if (window)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	return 0;
}

