cmake_minimum_required(VERSION 3.15...4.0)

project(
	tvrender
	VERSION 0.1.1
	LANGUAGES C
)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(
	tvrender
	src/cmd/cmdline.c
	src/common/id.c
	src/debug/print.c
	src/gl/src/gl.c
	src/io/tvr1.c
	src/io/tvr2.c
	src/io/tvrender.c
	src/main.c
	src/render/gfxenv.c
	src/render/gfxenv_gl.c
	src/render/gfxenv_gl_ib.c
	src/render/gfxenv_gl_mat.c
	src/render/gfxenv_gl_mesh.c
	src/render/gfxenv_gl_pov.c
	src/render/gfxenv_gl_tex.c
	src/render/gfxenv_gl_vb.c
	src/render/opengl2.c
	src/render/rflag.c
)

target_include_directories(
	tvrender
	PUBLIC src/gl/include
	PUBLIC /usr/include/libtvrender
	PUBLIC /usr/include/libdrm
)

find_library(CGLM cglm)
find_library(LIBGLFW glfw)
find_library(LIBTVR tvrender)
find_library(LIBDRM drm)
find_library(LIBM m)


target_link_libraries(tvrender ${CGLM} ${LIBGLFW} ${LIBTVR} ${LIBDRM} ${LIBM})

install(TARGETS tvrender)
install(DIRECTORY src/shaders DESTINATION $ENV{HOME}/.tvrender)

include(CPack)

