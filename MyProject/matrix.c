#include "matrix.h"
#include <math.h>

mat4_t mat4_zero()
{
	mat4_t m =
	{
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
	};
	return m;
}

mat4_t mat4_identity(void)
{
	mat4_t m =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
	};
	return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz)
{
	mat4_t m = mat4_identity();
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;
	return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
	vec4_t result;
	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
	return result;
}

mat4_t mat4_mul_mat4(mat4_t m1, mat4_t m2)
{
	mat4_t m;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			m.m[r][c] = 
				m1.m[r][0] * m2.m[0][c] + 
				m1.m[r][1] * m2.m[1][c] + 
				m1.m[r][2] * m2.m[2][c] + 
				m1.m[r][3] * m2.m[3][c];
		}
	}
	return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz)
{
	mat4_t m = mat4_identity();
	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;
	return m;
}

mat4_t mat4_make_rotation_x(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	// 1  0  0  0 
	// 0  c -s  0
	// 0  s  c  0
	// 0  0  0  1
	mat4_t m = mat4_identity();
	m.m[1][1] = c;
	m.m[1][2] = -s;
	m.m[2][1] = s;
	m.m[2][2] = c;
	return m;
}

mat4_t mat4_make_rotation_y(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	//  c  0  s  0 
	//  0  1  0  0
	// -s  0  c  0
	//  0  0  0  1
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][2] = s;
	m.m[2][0] = -s;
	m.m[2][2] = c;
	return m;
}

mat4_t mat4_make_rotation_z(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	//  c  -s  0  0 
	//  s   c  0  0
	//  0   0  1  0
	//  0   0  0  1
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][1] = -s;
	m.m[1][0] = s;
	m.m[1][1] = c;
	return m;
}

mat4_t make_world_matrix(mat4_t s, mat4_t r, mat4_t t)
{
	mat4_t m = mat4_identity();
	m = mat4_mul_mat4(m, s);
	m = mat4_mul_mat4(m, r);
	m = mat4_mul_mat4(m, t);
	return m;
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar)
{
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
	mat4_t m = mat4_zero();
	m.m[0][0] = aspect * atan(fov / 2);
	m.m[1][1] = atan(fov / 2);
	m.m[2][2] = zfar / (zfar - znear);
	m.m[3][3] = (-zfar * znear) / (zfar - znear);
	m.m[3][2] = 1.0f;
	return m;
}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v)
{
	vec4_t result = mat4_mul_vec4(mat_proj, v);

	// perspective divide
	if (result.w != 0.0f)
	{
		result.x /= result.w;
		result.y /= result.w;
		result.z /= result.w;
	}
	return result;
}
