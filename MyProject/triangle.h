#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include "texture.h"
#include <stdint.h>

// stores vertex index
typedef struct 
{
	int a;
	int b;
	int c;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	uint32_t color;
} face_t;

// stores the actual vec2 points of the triangle in the screen
typedef struct
{
	vec4_t points[3];
	tex2_t texcoords[3];
	uint32_t color;
	float avg_depth;
} triangle_t;

void draw_triangle(triangle_t* triangle, uint32_t color);
void draw_textured_triangle(triangle_t* triangle, uint32_t* texture);
void draw_triangle1(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_filled_triangle2(triangle_t* triangle, uint32_t color);

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color);

void fill_flat_bottom_triangle_texture(triangle_t* triangle, uint32_t texture);
void fill_flat_top_triangle_texture(triangle_t* triangle, uint32_t texture);

/// <summary>
/// Function to draw the textured pixel at position x and y using interpolation
/// </summary>
void draw_texel(int x, int y, uint32_t* texture, vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

#endif