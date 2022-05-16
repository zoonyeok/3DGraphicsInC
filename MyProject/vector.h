#pragma once
#ifndef VECTOR_H
#define VECTOR_H

typedef struct 
{
	float x;
	float y;
} vec2_t;

typedef struct 
{
	float x;
	float y;
	float z;
} vec3_t;

//TODO : Add fuctions to manipulate vector2D and vector3D

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

#endif // !VECTOR_H
