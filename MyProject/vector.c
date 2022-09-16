#include "vector.h"
#include <math.h>
//
/****************************
*  Vector2 Functions
****************************/
float vec2_length(vec2_t v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b)
{
	vec2_t result = 
	{ 
		.x = a.x + b.x, 
		.y = a.y + b.y 
	};
	return result;
}

vec2_t vec2_sub(vec2_t a, vec2_t b)
{
	vec2_t result = 
	{
		.x = a.x - b.x,
		.y = a.y - b.y
	};
	return result;
}

vec2_t vec2_mul(vec2_t v, float factor)
{
	vec2_t result = 
	{
		.x = v.x * factor,
		.y = v.y * factor
	};
	return result;
}

vec2_t vec2_div(vec2_t v, float factor)
{
	vec2_t result = 
	{
		.x = v.x / factor,
		.y = v.y / factor
	};
	return result;
}

float vec2_dot(vec2_t a, vec2_t b)
{
	return (a.x * b.x) + (a.y * b.y);
}

void vec2_normalize(vec2_t* v)
{
	float length = sqrt(v->x * v->x + v->y * v->y);
	v->x / length;
	v->y / length;
}

/****************************
*  Vector3 Functions
****************************/
float vec3_length(vec3_t v) 
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b) 
{
	vec3_t result = 
	{
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
	return result;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) 
{
	vec3_t result = 
	{
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
	return result;
}

vec3_t vec3_mul(vec3_t v, float factor) 
{
	vec3_t result = {
		.x = v.x * factor,
		.y = v.y * factor,
		.z = v.z * factor
	};
	return result;
}

vec3_t vec3_div(vec3_t v, float factor) 
{
	vec3_t result = {
		.x = v.x / factor,
		.y = v.y / factor,
		.z = v.z / factor
	};
	return result;
}

vec3_t vec3_cross(vec3_t a, vec3_t b)
{
	vec3_t result = 
	{
	   .x = a.y * b.z - a.z * b.y,
	   .y = a.z * b.x - a.x * b.z,
	   .z = a.x * b.y - a.y * b.x
	};
	return result;
}

/// <summary>
/// how aligned are two vectors each other
/// a.b = 1 ����
/// a.b > 0 
/// a.b = 0 ����
/// a.b < 0 
/// </summary>
float vec3_dot(vec3_t a, vec3_t b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vec3_normalize(vec3_t* v)
{
	float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= length;
	v->y /= length;
	v->z /= length;
}

vec3_t vec3_rotate_x(vec3_t v, float angle)
{
	vec3_t rotated_vector =
	{
		.x = v.x,
		.y = v.y * cos(angle) - v.z * sin(angle),
		.z = v.y * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle)
{
	vec3_t rotated_vector =
	{
		.x = v.x * cos(angle) + v.z * sin(angle),
		.y = v.y,
		.z = -v.x * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle)
{
	vec3_t rotated_vector =
	{
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z
	};
	return rotated_vector;
}

vec2_t vec2_from_vec4(vec4_t v)
{
	vec2_t result =
	{
		v.x,
		v.y
	};
	return result;
}

vec3_t vec3_from_vec4(vec4_t v)
{
	vec3_t result =
	{
		v.x,
		v.y,
		v.z
	};
	return result;
}

/****************************
*  Vector4 Functions
****************************/
vec4_t vec4_from_vec3(vec3_t v)
{
	vec4_t result =
	{
		v.x,
		v.y,
		v.z,
		1.0f
	};
	return result;
}

vec3_t make_normal(vec4_t transformed_vertices[])
{
	vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);	/*   A   */
	vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);	/*  / \  */
	vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);	/* C---B */

	// Get the vector 
	vec3_t vector_ab = vec3_sub(vector_b, vector_a);
	vec3_t vector_ac = vec3_sub(vector_c, vector_a);
	vec3_normalize(&vector_ab);
	vec3_normalize(&vector_ac);

	// Compute the face normal (using cross product to find perpendicular)
	vec3_t normal = vec3_cross(vector_ab, vector_ac); // ab ac ���� �߿�
	vec3_normalize(&normal);
	return normal;
}