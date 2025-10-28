#ifndef COMMON_MATHS_H
#define COMMON_MATHS_H 1

#define FILL_VEC4(n) {n, n, n, n}
#define FILL_MAT4(n) {FILL_VEC4(n), FILL_VEC4(n), FILL_VEC4(n), FILL_VEC4(n)}

enum
{
	X = 0,
	Y = 1,
	Z = 2,
	W = 3
};

typedef struct
{
	float x, y, z;
} Vec3;

typedef struct
{
	float x, y, z, w;
} Vec4;

typedef struct
{
	Vec4 x, y, z, w;
} Mat4;


int clamp(int n, int min, int max);
int max(int a, int b);
int min(int a, int b);

Vec3 crossVec3(Vec3 a, Vec3 b);
Vec3 normaliseVec3(Vec3 v);
float dotVec3(Vec3 a, Vec3 b);

Mat4 matLook(Vec3 eye, Vec3 centre, Vec3 up);
Mat4 matPersp(float fov, float aspect, float near, float far);

void scale3d(float (*m)[4], float x, float y, float z);
void rotate3d(float (*m)[4], float x, float y, float z);
void translate3d(float (*m)[4], float x, float y, float z);

Vec3 fwd_from_angles(float x, float y, float z);
Vec3 right_from_angles(float x, float y, float z);
Vec3 up_from_angles(float x, float y, float z);

void cross4(float (*m)[4], float (*a)[4], float (*b)[4]);

#endif

