#include "light.h"

light_t g_light = { .direction = {0.f, 0.f, 1.f} };

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor)
{
	/*if (percentage_factor < 0) percentage_factor = 0;
	if (percentage_factor > 1) percentage_factor = 1;*/

	uint32_t a = (original_color & 0xFF000000);
	uint32_t r = (original_color & 0x00FF0000) * percentage_factor;
	uint32_t g = (original_color & 0x0000FF00) * percentage_factor;
	uint32_t b = (original_color & 0x000000FF) * percentage_factor;

	return  a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

float light_intensity_factor(vec4_t transformed_vertices[])
{
	return -vec3_dot(make_normal(transformed_vertices), g_light.direction);
}