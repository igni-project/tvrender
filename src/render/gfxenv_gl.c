#include "gfxenv.h"
#include "opengl.h"
#include "../debug/print.h"
#include "../io/tvrender.h"
#include "../common/id.h"
#include "rflag.h"

#include <cglm/clipspace/view_lh_no.h>
#include <tvr_core.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

char vert_path[80] = {0};
char frag_path[80] = {0};

char error_tex_data[3] = {255, 0, 255};

int create_gfxenv_gl(struct gfxenv *gfxenv, GLFWwindow *window)
{
	char info_log[256] = {0};
	int success;

	GLuint vert_shader;
	GLuint frag_shader;
	GLuint program;
	int gl_version;
	GLenum error_code;

	const char *home_dir;

	/* Find home directory where shaders are located */
	home_dir = getenv("HOME");
	if (!home_dir)
	{
		perror("Failed to find home directory");
	}

	strcat(vert_path, home_dir);
	strcat(vert_path, "/.tvrender/shaders/vert.vert");
	strcat(frag_path, home_dir);
	strcat(frag_path, "/.tvrender/shaders/frag.frag");

	/* There is no OpenGL graphics without a display surface. */
	if (!window)
	{
		printf("Could not init OpenGL: No window detected\n");
		return -1;
	}

	/* Load in the OpenGL library */
	gl_version = gladLoadGL(glfwGetProcAddress);
	if (gl_version == 0) {
		printf("Failed to initialize OpenGL context\n");
		return -1;
	}

	/* Activate Z-buffering */
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_gl);

	/* Shaders */

	if (gl_load_shader(vert_path, GL_VERTEX_SHADER, &vert_shader) == -1)
	{
		printf("Could not create vertex shader.\n");
		return -1;
	}

	if (gl_load_shader(frag_path, GL_FRAGMENT_SHADER, &frag_shader) == -1)
	{
		printf("Could not create fragment shader.\n");
		return -1;
	}

	program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, info_log);
		printf("Could not link shader program.\n", info_log);

		glDeleteProgram(program);
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);  

		return -1;
	}

	glUseProgram(program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);  

	gfxenv->data.gl.program = program;

	/* Set up uniform buffer objects */

	/* UBO 1. transformation matrix */
	gfxenv->data.gl.tf_ubo = glGetUniformLocation(program, "transform");
	
	error_code = glGetError();

	if (error_code)
	{
		printf("Failed to get transformation UBO.\n");
		printf("OpenGL error code %i\n", error_code);

		return -1;
	}

	/* UBO 2. projection matrix */
	gfxenv->data.gl.proj_ubo = glGetUniformLocation(program, "projection");
	
	error_code = glGetError();

	if (error_code)
	{
		printf("Failed to get projection UBO.\n");
		printf("OpenGL error code %i\n", error_code);

		return -1;
	}

	/* UBO 3. view matrix */
	gfxenv->data.gl.view_ubo = glGetUniformLocation(program, "view");
		
	error_code = glGetError();

	if (error_code)
	{
		printf("Failed to get view UBO.\n");
		printf("OpenGL error code %i\n", error_code);

		return -1;
	}

	/* Generate default error texture */
	glGenTextures(1, &gfxenv->data.gl.error_tex);  
	glBindTexture(GL_TEXTURE_2D, gfxenv->data.gl.error_tex);  
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		1,
		1,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		error_tex_data
	);
	glGenerateMipmap(GL_TEXTURE_2D);	

	/* Position camera */

	glm_mat4_identity(gfxenv->data.gl.pov.proj);
	glm_mat4_identity(gfxenv->data.gl.pov.view);

	vec3 eye = {0.0f}, centre = {0.0f};
	eye[2] = 10.0f;
	eye[1] = 10.0f;
	eye[0] = 10.0f;
	gfxenv->data.gl.pov.up[2] = 1.0f;

	vec3 ctest = {0.0f};

	glm_vec3_sub(centre, eye, gfxenv->data.gl.pov.fwd);
	glm_vec3_normalize(gfxenv->data.gl.pov.fwd);

	glm_vec3_add(eye, gfxenv->data.gl.pov.fwd, ctest);

	glm_lookat(
		eye,
		ctest,
		gfxenv->data.gl.pov.up,
		gfxenv->data.gl.pov.view
	);
	glm_perspective( 
		glm_rad(90.0f), /* deg to radian conversion */ 
		800.0f / 600.0f, 
		0.1f, 
		100.0f,
		gfxenv->data.gl.pov.proj
	);

	/* Set up projection UBO */ 
	glUniformMatrix4fv(
		gfxenv->data.gl.proj_ubo,
		1,
		GL_FALSE,
		(GLfloat*)&gfxenv->data.gl.pov.proj
	);

	/* Set up view UBO */ 
	glUniformMatrix4fv(
		gfxenv->data.gl.view_ubo,
		1,
		GL_FALSE,
		(GLfloat*)&gfxenv->data.gl.pov.view
	);

	/* Reassign engine-specific function pointers */
	p_gfxenv_render = &gfxenv_render_gl;
	p_destroy_gfxenv = &destroy_gfxenv_gl;
	p_gfxenv_new_client = &gfxenv_new_client_gl;
	p_gfxenv_exec = &gfxenv_exec_gl;

	/* Allocate scene array */
	gfxenv->data.gl.scenes = malloc(sizeof(struct gl_scene));
	
	return 0;
}

int gfxenv_render_gl(struct gfxenv *gfxenv, GLFWwindow *window)
{
	struct gl_scene gls;
	int mesh_counter = 0;
	int scene_counter = 0;

	if (!needs_redraw())
	{
		return 0;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while (scene_counter < gfxenv->scene_count)
	{
		gls = gfxenv->data.gl.scenes[scene_counter];
	
		while (mesh_counter < gls.mesh_count)
		{
			glUseProgram(gfxenv->data.gl.program);
		
			/* UBO */
			glUniformMatrix4fv(
				gfxenv->data.gl.tf_ubo,
				1,
				GL_FALSE,
				(GLfloat*)&gls.meshes[mesh_counter].tf
			);

			/* Albedo texture */
			glBindTexture(GL_TEXTURE_2D, gls.meshes[mesh_counter].albedo);

			/* Vertices & indices */
			glBindVertexArray(gls.meshes[mesh_counter].vertex_array);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

			glBindVertexArray(0);

			++mesh_counter;
		}

		++scene_counter;
	}
	
	/* Announce that the latest frame has been drawn */
	unflag_redraw();

	glfwSwapBuffers(window);

	return 0;
}

void destroy_gfxenv_gl(struct gfxenv gfxenv)
{
	glDeleteTextures(1, &gfxenv.data.gl.error_tex);

	glDeleteProgram(gfxenv.data.gl.program);

	free(gfxenv.data.gl.scenes);
	free(gfxenv.scenes);
}

int gfxenv_new_client_gl(struct gfxenv *gfxenv, int fd)
{
	void *new_addr;
	struct gl_gfxenv *gl;
	struct gl_scene *gls;

	gl = &gfxenv->data.gl;

	if (gfxenv->scene_count >= gfxenv->scene_limit)
	{
		gfxenv->scene_limit *= 2;

		/* Reallocate OpenGL scene array */
		new_addr = realloc(
				gl->scenes,
				gfxenv->scene_limit * sizeof(struct gl_scene)
		);

		/* Always check reallocated addresses. */
		if (!new_addr)
		{
			perror("new_client_gl() failed: could not allocate memory");
			return -1;
		}

		gl->scenes = (struct gl_scene *)new_addr;

		/* Reallocate GFXenv scene array */
		new_addr = realloc(
				gfxenv->scenes, 
				gfxenv->scene_limit * sizeof(struct gfxenv_scene)
		);

		if (!new_addr)
		{
			perror("new_client_gl() failed: could not allocate memory");
			return -1;
		}

		gfxenv->scenes = (struct gfxenv_scene *)new_addr;
	}


	gfxenv->scenes[gfxenv->scene_count].fd = fd;

	/* Initialise the new scene's dynamic arrays */

	gls = &gl->scenes[gfxenv->scene_count];

	/* Vertex buffer array */

	gls->vertex_buffers = malloc(sizeof(struct gl_buffer));

	if (!gls->vertex_buffers)
	{
		perror("failed to allocate space for vertex buffers");
		return -1;
	}

	gls->vertex_buffer_ids = malloc(sizeof(int));

	if (!gls->vertex_buffer_ids)
	{
		perror("failed to allocate space for vertex buffer IDs");
		return -1;
	}

	gls->vertex_buffer_count = 0;
	gls->vertex_buffer_lim = 1;

	/* Index buffer array */

	gls->index_buffers = malloc(sizeof(struct gl_buffer));

	if (!gls->index_buffers)
	{
		perror("failed to allocate space for index buffers");
		return -1;
	}

	gls->index_buffer_ids = malloc(sizeof(int));

	if (!gls->index_buffer_ids)
	{
		perror("failed to allocate space for index buffer IDs");
		return -1;
	}

	gls->index_buffer_count = 0;
	gls->index_buffer_lim = 1;

	/* Mesh array */

	gls->meshes = malloc(sizeof(struct gl_mesh));

	if (!gls->meshes)
	{
		perror("failed to allocate space for meshes");
		return -1;
	}

	gls->mesh_ids = malloc(sizeof(int));
	
	if (!gls->mesh_ids)
	{
		perror("failed to allocate space for mesh IDs");
		return -1;
	}

	gls->mesh_count = 0;
	gls->mesh_lim = 1;

	/* Texture array */

	gls->textures = malloc(sizeof(struct gl_texture));

	if (!gls->textures)
	{
		perror("failed to allocate space for textures");
		return -1;
	}

	gls->texture_ids = malloc(sizeof(int));

	if (!gls->texture_ids)
	{
		perror("failed to allocate space for texture IDs");
		return -1;
	}

	gls->texture_count = 0;
	gls->texture_lim = 1;

	/* Material array */

	gls->mats = malloc(sizeof(struct gl_material));

	
	if (!gls->mats)
	{
		perror("failed to allocate space for index buffers");
		return -1;
	}

	gls->mat_ids = malloc(sizeof(int));

	if (!gls->mat_ids)
	{
		perror("failed to allocate space for index buffer IDs");
		return -1;
	}

	gls->mat_count = 0;
	gls->mat_lim = 1;

	++gfxenv->scene_count;

	db_print_vb("\033[1mNew client\033[0m\n");
	db_print_vb("| Client ID: %i\n", fd);

	return 0;
}

int gfxenv_exec_gl(struct gfxenv *gfxenv, unsigned int scene)
{
	int ret;
	/* The TVrender protocol has 16-bit opcodes. */
	uint16_t opcode;
	int fd;

	/* The scene will change so flag for a re-render */
	flag_redraw();

	if (scene > gfxenv->scene_count)
	{
		printf("Error: scene index out of range!\n");
		return -1;
	}

	fd = gfxenv->scenes[scene].fd;

	ret = recv(fd, &opcode, sizeof(opcode), 0);
	if (ret == -1)
	{
		perror("An error occurred while receiving data");
		
		gfxenv_del_scene_gl(gfxenv, scene);
	
		return -1;
	}

	/* A message size of 0 means client has disconnected */
	if (!ret)
	{
		gfxenv_del_scene_gl(gfxenv, scene);
		
		return 0;
	}

	/* The real core of this function.
	 * A switch statement covering every TVrender opcode. */
	switch (opcode)
	{
	case TVR_OPCODE_SET_VERSION:
		ret = gfxenv_set_tvr_version(gfxenv, scene);
		break;

	case TVR_OPCODE_VERTEX_BUFFER_CREATE:
		ret = gfxenv_create_vertex_buffer_gl(gfxenv, scene);
		break;
	case TVR_OPCODE_VERTEX_BUFFER_DESTROY:
		ret = gfxenv_destroy_vertex_buffer_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_INDEX_BUFFER_CREATE:
		ret = gfxenv_create_index_buffer_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_INDEX_BUFFER_DESTROY:
		ret = gfxenv_destroy_index_buffer_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MESH_CREATE:
		ret = gfxenv_create_mesh_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MESH_BIND_MAT:
		ret = gfxenv_mesh_bind_mat_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MESH_SET_LOC:
		ret = gfxenv_mesh_set_loc_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MESH_SET_ROT:
		ret = gfxenv_mesh_set_rot_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MESH_SET_SCALE:
		ret = gfxenv_mesh_set_scale_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MESH_DESTROY:
		ret = gfxenv_destroy_mesh_gl(gfxenv, scene);
		break;


	case TVR_OPCODE_TEXTURE_CREATE:
		ret = gfxenv_create_texture_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_TEXTURE_DESTROY:
		ret = gfxenv_destroy_texture_gl(gfxenv, scene);
		break;


	case TVR_OPCODE_MATERIAL_CREATE:
		ret = gfxenv_create_material_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MATERIAL_BIND_TEXTURE:
		ret = gfxenv_material_bind_texture_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_MATERIAL_DESTROY:
		ret = gfxenv_destroy_material_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_POV_SET_LOC:
		ret = gfxenv_set_pov_loc_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_POV_SET_ROT:
		ret = gfxenv_set_pov_rot_gl(gfxenv, scene);
		break;

	case TVR_OPCODE_POV_SET_FOV:
		ret = gfxenv_set_pov_fov_gl(gfxenv, scene);
		break;

	default:
		printf("Unknown opcode: %i\n", opcode);
		gfxenv_del_scene_gl(gfxenv, scene);
		return -1;
	}

	if (ret == -1)
	{
		db_print_vb("Request failed.\n");
		gfxenv_del_scene_gl(gfxenv, scene);
		return -1;
	}

	return 0;
}

int gfxenv_del_scene_gl(
	struct gfxenv *gfxenv,
	unsigned int scene
)
{
	void *new_addr;

	/* Delete contents of scene */
	destroy_gl_scene(gfxenv->data.gl.scenes[scene]);

	/* Remove deleted scene from scene array */

	if (scene > gfxenv->scene_count)
	{
		db_print_vb("Scene index out of range! (%i)\n", scene);
		return -1;
	}

	close(gfxenv->scenes[scene].fd);

	db_print_vb("| Client ID: %i\n", gfxenv->scenes[scene].fd);

	/* Shorten scene arrays + remove now unused entry */	
	--gfxenv->scene_count;

	memcpy(
		&gfxenv->scenes[scene],
		&gfxenv->scenes[scene + 1],
		(gfxenv->scene_count - scene) * sizeof(struct gfxenv_scene)
	);

	memcpy(
		&gfxenv->data.gl.scenes[scene],
		&gfxenv->data.gl.scenes[scene + 1],
		(gfxenv->scene_count - scene) * sizeof(struct gl_scene)
	);

	if (gfxenv->scene_count < gfxenv->scene_limit / 2)
	{
		gfxenv->scene_limit /= 2;

		/* Reallocate scene array */
		new_addr = realloc(
				gfxenv->scenes,
				gfxenv->scene_limit * sizeof(struct gfxenv_scene)
		);

		if (!new_addr)
		{
			perror("realloc() in gfxenv_del_scene_gl() failed:");
			return -1;
		}

		gfxenv->scenes = (struct gfxenv_scene *)new_addr;

		/* Reallocate OpenGL scene array */
		new_addr = realloc(
				gfxenv->data.gl.scenes,
				gfxenv->scene_limit * sizeof(struct gl_scene)
		);

		if (!new_addr)
		{
			perror("realloc() in gfxenv_del_scene_gl() failed:");
			return -1;
		}

		gfxenv->data.gl.scenes = (struct gl_scene *)new_addr;
	}
	
	return 0;

}

