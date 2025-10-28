#include "opengl.h"
#include "rflag.h"
#include "../debug/print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int g_width, g_height;

int gl_load_shader(const char *path, GLenum shader_type, unsigned int *shader)
{
	char info_log[512];
	int success;
	struct stat filestat;
	char *code;
	GLint code_size;
	FILE *file = fopen(path, "r");

	if (!file)
	{
		perror("Failed to open shader file");
		return -1;
	}

	if (stat(path, &filestat) == -1)
	{
		perror("Failed to get shader file stats");
		return -1;
	}

	code = malloc(filestat.st_size);

	if (!code)
	{
		perror("Failed to allocate memory for shader code");
		return -1;
	}

	if (!fread(code, 1, filestat.st_size, file))
	{
		/* Bodged formatted error print */
		printf("Failed to read shader file (%s)", path);
		perror("");
		free(code);
		return -1;
	}

	*shader = glCreateShader(shader_type);
	code_size = (GLint)filestat.st_size;
	glShaderSource(*shader, 1, (const char * const*)&code, &code_size);
	glCompileShader(*shader);

	free(code);
	fclose(file);

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(*shader, 512, NULL, info_log);
		printf("Could not compile shader %s\n%s\n", path, info_log);
		glDeleteShader(*shader);

		return -1;
	}

	return 0;
}

/* delete a gl_scene object */
int destroy_gl_scene(struct gl_scene gls)
{
	void *new_addr;
	int count;

	db_print_vb("\033[1mClient disconnect\033[0m\n");

	/* Delete vertex buffers */

	count = 0;
	while (count < gls.vertex_buffer_count)
	{
		glDeleteBuffers(1, &gls.vertex_buffers[count].buf);
		++count;
	}

	free(gls.vertex_buffers);
	free(gls.vertex_buffer_ids);

	/* Delete index buffers */

	count = 0;
	while (count < gls.index_buffer_count)
	{
		glDeleteBuffers(1, &gls.index_buffers[count].buf);
		++count;
	}

	free(gls.index_buffers);
	free(gls.index_buffer_ids);

	/* Delete meshes */

	count = 0;
	while (count < gls.mesh_count)
	{
		glDeleteVertexArrays(1, &gls.meshes[count].vertex_array);
		++count;
	}

	free(gls.meshes);
	free(gls.mesh_ids);

	/* Delete textures */

	free(gls.textures);
	free(gls.texture_ids);
	
	/* Delete materials */

	free(gls.mats);
	free(gls.mat_ids);

	return 0;
}

void framebuffer_size_callback_gl(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	g_width = width;
	g_height = height;
	flag_redraw();
}

int get_screen_width()
{
	return g_width;
}

int get_screen_height()
{
	return g_height;
}

