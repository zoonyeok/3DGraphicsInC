#include "triangle.h"
#include "color.h"
#include "display.h"

void int_swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void float_swap(float* a, float* b)
{
	float tmp = *a;
	*a = *b;
	*b = tmp;
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	// Find the two slopes (two triagnle legs)
	float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
	float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

	// Start x_start and x_end from the top vertex (x0, y0)
	float x_start = x0;
	float x_end = x0;

	// Loop all the scanline from top to bottom
	for (int y = y0; y <= y2; y++)
	{
		draw_line(x_start, y, x_end, y, color);
		x_start += inv_slope_1;
		x_end += inv_slope_2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color)
{
	float inv_slope_1 = (float)(x2 - x1) / (y2 - y1);
	float inv_slope_2 = (float)(x2 - Mx) / (y2 - My);

	float x_start = x2;
	float x_end = x2;

	for (int y = y2; y >= y1; y--)
	{
		draw_line(x_start, y, x_end, y, color);
		x_start -= inv_slope_1;
		x_end -= inv_slope_2;
	}
}


void fill_flat_bottom_triangle_texture(triangle_t* triangle, uint32_t texture)
{
	int x0 = (int)triangle->points[0].x; int y0 = (int)triangle->points[0].y;
	int x1 = (int)triangle->points[1].x; int y1 = (int)triangle->points[1].y;
	int x2 = (int)triangle->points[2].x; int y2 = (int)triangle->points[2].y;

	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	// Start x_start and x_end from the top vertex (x0, y0)
	float x_start = x0;
	float x_end = x0;

	// Loop all the scanline from top to bottom
	if (y1 - y0 != 0)
	{
		for (int y = y0; y <= y1; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			/*	int x_min = x_start > x_end ? x_end : x_start;
				int x_max = x_start < x_end ? x_end : x_start;*/

			if (x_end < x_start)
				int_swap(&x_start, &x_end);

			for (int x = x_start; x < x_end; x++)
			{
				uint32_t color = Red;
				draw_pixel(x, y, color);
			}
		}
	}
}

void fill_flat_top_triangle_texture(triangle_t* triangle, uint32_t texture)
{
	int x0 = (int)triangle->points[0].x; int y0 = (int)triangle->points[0].y;
	int x1 = (int)triangle->points[1].x; int y1 = (int)triangle->points[1].y;
	int x2 = (int)triangle->points[2].x; int y2 = (int)triangle->points[2].y;

	////////////////////////////////////
	// flat top
	////////////////////////////////////

	// Find the two slopes (two triagnle legs)
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	// Loop all the scanline from top to bottom
	if (y2 - y1 != 0)
	{
		for (int y = y1; y <= y2; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start)
				int_swap(&x_start, &x_end);

			for (int x = x_start; x < x_end; x++)
			{
				uint32_t color = Red;
				draw_pixel(x, y, color);
			}
		}
	}
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
	// Find the vectors between the vertices ABC and point p
	vec2_t ab = vec2_sub(b, a);
	vec2_t bc = vec2_sub(c, b);
	vec2_t ac = vec2_sub(c, a);
	vec2_t ap = vec2_sub(p, a);
	vec2_t bp = vec2_sub(p, b);

	// Calcualte the area of the full triangle ABC using cross product (area of parallelogram)
	float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);

	// Weight alpha is the area of subtriangle BCP divided by the area of the full triangle ABC
	float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;

	// Weight beta is the area of subtriangle ACP divided by the area of the full triangle ABC
	float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;

	// Weight gamma is easily found since barycentric cooordinates always add up to 1
	float gamma = 1 - alpha - beta;

	vec3_t weights = { alpha, beta, gamma };
	return weights;
}

void draw_texel(int x, int y, uint32_t* texture, vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);

	vec3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// Variables to store the interpolated values of U, V, and also 1/w for the current pixel
	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w;

	// Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
	interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
	interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

	// Also interpolate the value of 1/w for the current pixel
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	// Now we can divide back both interpolated values by 1/w
	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	// Map the UV coordinate to the full texture width and height
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

	draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle2(triangle_t* tri, uint32_t color)
{
	int x0 = (int)tri->points[0].x; int y0 = (int)tri->points[0].y;
	int x1 = (int)tri->points[1].x; int y1 = (int)tri->points[1].y;
	int x2 = (int)tri->points[2].x; int y2 = (int)tri->points[2].y;

	// we need to sort the vertices by y-coordinate ascending ( y0 < y1 < y2 )
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}
	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
	}
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}

	// Avoiding division zero
	if (y1 == y2)
	{
		// 밑에 삼각형이 없는 경우
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else if (y0 == y1)
	{
		// 위에 삼각형이 없는 경우
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else
	{
		// Caculate the new vertex (Mx, My) using triangle similarity
		int My = y1;
		int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

		// Draw flat-bottom triangle
		fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

		// Draw flat-top triangle
		fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
	}
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	// we need to sort the vertices by y-coordinate ascending ( y0 < y1 < y2 )
	if (y0 > y1)
	{
		float_swap(&y0, &y1);
		float_swap(&x0, &x1);
	}
	if (y1 > y2)
	{
		float_swap(&y1, &y2);
		float_swap(&x1, &x2);
	}
	if (y0 > y1)
	{
		float_swap(&y0, &y1);
		float_swap(&x0, &x1);
	}

	if (y1 == y2)
	{
		// 밑에 삼각형이 없는 경우
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else if (y0 == y1)
	{
		// 위에 삼각형이 없는 경우
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else
	{
		// Caculate the new vertex (Mx, My) using triangle similarity
		int My = y1;
		int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

		// Draw flat-bottom triangle
		fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

		// Draw flat-top triangle
		fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draw a triangle using three raw line calls
///////////////////////////////////////////////////////////////////////////////
void draw_triangle1(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

void draw_triangle(triangle_t* triangle, uint32_t color)
{
	for (int i = 0; i < 2; i++)
	{
		draw_line(triangle->points[i].x, triangle->points[i].y,
			triangle->points[i + 1].x, triangle->points[i + 1].y, color);
	}
	draw_line(triangle->points[2].x, triangle->points[2].y,
		triangle->points[0].x, triangle->points[0].y, color);
}

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(triangle_t* triangle, uint32_t* texture)
{
	int x0 = (int)triangle->points[0].x; int y0 = (int)triangle->points[0].y;
	int x1 = (int)triangle->points[1].x; int y1 = (int)triangle->points[1].y;
	int x2 = (int)triangle->points[2].x; int y2 = (int)triangle->points[2].y;

	float z0 = triangle->points[0].z; float w0 = triangle->points[0].w;
	float z1 = triangle->points[1].z; float w1 = triangle->points[1].w;
	float z2 = triangle->points[2].z; float w2 = triangle->points[2].w;

	float u0 = triangle->texcoords[0].u; float u1 = triangle->texcoords[1].u; float u2 = triangle->texcoords[2].u;
	float v0 = triangle->texcoords[0].v; float v1 = triangle->texcoords[1].v; float v2 = triangle->texcoords[2].v;

	// we need to sort the vertices by y-coordinate ascending ( y0 < y1 < y2 )
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}
	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
	}
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}

	// flip the u component
	v0 = 1.0f - v0;
	v1 = 1.0f - v1;
	v2 = 1.0f - v2;

	vec4_t point_a = { x0, y0, z0, w0 };
	vec4_t point_b = { x1, y1, z1, w1 };
	vec4_t point_c = { x2, y2, z2, w2 };
	tex2_t a_uv = { u0, v0 };
	tex2_t b_uv = { u1, v1 };
	tex2_t c_uv = { u2, v2 };

	////////////////////////////////////
	// flat bottom
	////////////////////////////////////

	// Find the two slopes (two triagnle legs)
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	// Loop all the scanline from top to bottom
	if (y1 - y0 != 0)
	{
		for (int y = y0; y <= y1; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start)
				int_swap(&x_start, &x_end);

			for (int x = x_start; x < x_end; x++)
			{
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
			}
		}
	}

	////////////////////////////////////
	// flat top
	////////////////////////////////////

	// Find the two slopes (two triagnle legs)
	inv_slope_1 = 0;
	inv_slope_2 = 0;

	if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	// Loop all the scanline from top to bottom
	if (y2 - y1 != 0)
	{
		for (int y = y1; y <= y2; y++)
		{
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start)
				int_swap(&x_start, &x_end);

			for (int x = x_start; x < x_end; x++)
			{
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
			}
		}
	}
}
