#include "maths.h"
#include <math.h>

int clamp(int n, int min, int max)
{
	register int result = n < min ? min : n; /* Clip lower bounds */
	return result > max ? max : n; /* Clip upper bounds */
}   

int max(int a, int b)
{
	return a > b ? a : b;
}

int min(int a, int b)
{
	return a < b ? a : b;
}

Vec3 crossVec3(Vec3 a, Vec3 b)
{
	Vec3 result = {};

	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;

	return result;
}

Vec3 normaliseVec3(Vec3 v)
{
	Vec3 result = {};

	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	result.x = v.x / len;
	result.y = v.y / len;
	result.z = v.z / len;

	return result;
}

float dotVec3(Vec3 a, Vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Mat4 matLook(Vec3 eye, Vec3 centre, Vec3 up)
{
	Vec3 X, Y, Z = {0.0f};

	Z.x = eye.x - centre.x;
	Z.y = eye.y - centre.y;
	Z.z = eye.z - centre.z;
	Z = normaliseVec3(Z);

	Y = up;
	X = crossVec3(Y, Z);
	Y = crossVec3(Z, X);
	X = normaliseVec3(X);
	Y = normaliseVec3(Y);

	Mat4 m = {0.0f};

	m.x.x = X.x;
	m.y.x = X.y;
	m.z.x = X.z;
	m.w.x = -dotVec3(X, eye);

	m.x.y = Y.x;
	m.y.y = Y.y;
	m.z.y = Y.z;
	m.w.y = -dotVec3(Y, eye);

	m.x.z = Z.x;
	m.y.z = Z.y;
	m.z.z = Z.z;
	m.w.z = -dotVec3(Z, eye);

	m.x.w = 0.0f;
	m.y.w = 0.0f;
	m.z.w = 0.0f;
	m.w.w = 1.0f;

	return m;
}


/* Generate a perspective transformation matrix */
Mat4 matPersp(float fov, float aspect, float near, float far)
{
	float top = tanf(fov / 2.0f);
	float right = top * aspect;

	Mat4 m = {0.0f};

	m.x.x = near / right;
	m.y.y = -near / top;
	m.z.z = -(far + near) / (far - near);
	m.z.w = -1.0f;
	m.w.z = -(2.0f * far + near) / (far - near);

	return m;
}

void scale3d(float (*m)[4], float x, float y, float z)
{
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	m[3][3] = 1.0f;
}

void rotate3d(float (*m)[4], float x, float y, float z)
{
	const float sx = sinf(x);
	const float sy = sinf(y);
	const float sz = sinf(z);
	const float cx = cosf(x);
	const float cy = cosf(y);
	const float cz = cosf(z);

	/*						  Rotation matrices
	*	  X matrix (roll)	 Y matrix (pitch)	Z matrix (yaw)
	*	  | 1	 0   0   |   | cy	0    sy |   | cz	-sz	0  |
	*	  | 0	 cx  -sx | * | 0	 1   0  | * | sz	cz	 0 |
	*	  | 0	 sx  cx  |   | -sy   0   cy |   | 0	 0	  1    |
	*
	*					Combined rotation matrix
	*	  | cz cy	 sx sy cz - cx cz	cx sy cz + sx cz |
	*	= | sz sy	 sx sy sz + cx cz	cx sy xz - sx cz |
	*	  | -sy		 sx cy				cx cy			 |
	*/

	m[0][0] *= cz * cy;
	m[0][1] = sz * cy;
	m[0][2] = -sy;

	m[1][0] = sx * sy * cz - cx * sz;
	m[1][1] *= sx * sy * sz + cx * cz;
	m[1][2] = sx * cy;

	m[2][0] = cx * sy * cz + sx * sz;
	m[2][1] = cx * sy * sz - sx * cz;
	m[2][2] *= cx * cy;
}

void translate3d(float (*m)[4], float x, float y, float z)
{
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	m[3][3] = 1.0f;
}

void translateMat43d(Mat4 *m, float x, float y, float z)
{
	m->w.x = x;
	m->w.y = y;
	m->w.z = z;
	m->w.w = 1.0f;
}

Vec3 fwd_from_angles(float x, float y, float z)
{
	Vec3 result;


	return result;
}

Vec3 right_from_angles(float x, float y, float z)
{
	Vec3 result;


	return result;
}

Vec3 up_from_angles(float x, float y, float z)
{
	Vec3 fwd;
	Vec3 right;

	fwd = fwd_from_angles(x, y, z);
	right = right_from_angles(x, y, z);

	return crossVec3(fwd, right);
}


void cross4(float (*m)[4], float (*a)[4], float (*b)[4])
{
	m[0][0]=a[0][0]*b[0][0]+a[1][0]*b[0][1]+a[2][0]*b[0][2]+a[3][0]*b[0][3];
	m[1][0]=a[0][0]*b[1][0]+a[1][0]*b[1][1]+a[2][0]*b[1][2]+a[3][0]*b[1][3];
	m[2][0]=a[0][0]*b[2][0]+a[1][0]*b[2][1]+a[2][0]*b[2][2]+a[3][0]*b[2][3];
	m[3][0]=a[0][0]*b[3][0]+a[1][0]*b[3][1]+a[2][0]*b[3][2]+a[3][0]*b[3][3];

	m[0][1]=a[0][1]*b[0][0]+a[1][1]*b[0][1]+a[2][1]*b[0][2]+a[3][1]*b[0][3];
	m[1][1]=a[0][1]*b[1][0]+a[1][1]*b[1][1]+a[2][1]*b[1][2]+a[3][1]*b[1][3];
	m[2][1]=a[0][1]*b[2][0]+a[1][1]*b[2][1]+a[2][1]*b[2][2]+a[3][1]*b[2][3];
	m[3][1]=a[0][1]*b[3][0]+a[1][1]*b[3][1]+a[2][1]*b[3][2]+a[3][1]*b[3][3];

	m[0][2]=a[0][2]*b[0][0]+a[1][2]*b[0][1]+a[2][2]*b[0][2]+a[3][2]*b[0][3];
	m[1][2]=a[0][2]*b[1][0]+a[1][2]*b[1][1]+a[2][2]*b[1][2]+a[3][2]*b[1][3];
	m[2][2]=a[0][2]*b[2][0]+a[1][2]*b[2][1]+a[2][2]*b[2][2]+a[3][2]*b[2][3];
	m[3][2]=a[0][2]*b[3][0]+a[1][2]*b[3][1]+a[2][2]*b[3][2]+a[3][2]*b[3][3];

	m[0][3]=a[0][3]*b[0][0]+a[1][3]*b[0][1]+a[2][3]*b[0][2]+a[3][3]*b[0][3];
	m[1][3]=a[0][3]*b[1][0]+a[1][3]*b[1][1]+a[2][3]*b[1][2]+a[3][3]*b[1][3];
	m[2][3]=a[0][3]*b[2][0]+a[1][3]*b[2][1]+a[2][3]*b[2][2]+a[3][3]*b[2][3];
	m[3][3]=a[0][3]*b[3][0]+a[1][3]*b[3][1]+a[2][3]*b[3][2]+a[3][3]*b[3][3];
}


