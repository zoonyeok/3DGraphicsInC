#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "color.h"
#include "triangle.h"

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { 0.0f, 0.0f, 0.0f };
float fov_factor = 640;

uint32_t vertex_color = Yellow;

bool is_running = false;
uint32_t previous_frame_time = 0;

void setup(void) 
{
	render_method = RENDER_WIRE;
	cull_method = CULL_BACKFACE;

	// Allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

	// Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	 load_cube_mesh_data();
	 //load_obj_file_data("cube.obj"); // ./assets/cube.obj
}

void process_input(void) 
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) 
	{
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
			is_running = false;
		if (event.key.keysym.sym == SDLK_1)
		{
			render_method = RENDER_WIRE_VERTEX;
			vertex_color = Red;
		}
		if (event.key.keysym.sym == SDLK_2)
			render_method = RENDER_WIRE;
		if (event.key.keysym.sym == SDLK_3)
			render_method = RENDER_FILL_TRIANGLE;
		if (event.key.keysym.sym == SDLK_4)
			render_method = RENDER_FILL_TRIANGLE_WIRE;
		if (event.key.keysym.sym == SDLK_c)
			cull_method = CULL_BACKFACE;
		if (event.key.keysym.sym == SDLK_d)
			cull_method = CULL_NONE;
		break;
	}
}

vec2_t project(vec3_t point)
{
	vec2_t projected_point = 
	{
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};

	return projected_point;
}

bool backface_culling(vec3_t transformed_vertices[])
{
	vec3_t vector_a = transformed_vertices[0];	/*   A   */
	vec3_t vector_b = transformed_vertices[1];	/*  / \  */
	vec3_t vector_c = transformed_vertices[2];	/* C---B */

	// Get the vector 
	vec3_t vector_ab = vec3_sub(vector_b, vector_a);
	vec3_t vector_ac = vec3_sub(vector_c, vector_a);
	vec3_normalize(&vector_ab);
	vec3_normalize(&vector_ac);

	// Compute the face normal (using cross product to find perpendicular)
	vec3_t normal = vec3_cross(vector_ab, vector_ac); // ?? ab ac 순서 중요
	vec3_normalize(&normal);

	vec3_t camera_ray = vec3_sub(camera_position, vector_a);

	if (vec3_dot(normal, camera_ray) < 0)
	{
		return true;
	}
	return false;
}

void update(void) 
{
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));
	
	// Wait some time until the reach the target frame time in milliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	// Only delay execution if we are running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) 
	{
		SDL_Delay(time_to_wait);
	}

	previous_frame_time = SDL_GetTicks();

	g_mesh2.rotation.x += 0.01f;
	g_mesh2.rotation.y += 0.01f;
	g_mesh2.rotation.z += 0.01f;

	// 배열 초기화
	triangles_to_render = NULL;

	int num_faces = array_length(g_mesh2.faces);
	// 모든 triangle face 순회
	for (int i = 0; i < num_faces; i++) 
	{
		face_t mesh_face = g_mesh2.faces[i];
		
		vec3_t face_vertices[3];
		face_vertices[0] = g_mesh2.vertices[mesh_face.a - 1]; 
		face_vertices[1] = g_mesh2.vertices[mesh_face.b - 1]; 
		face_vertices[2] = g_mesh2.vertices[mesh_face.c - 1]; 

		vec3_t transformed_vertices[3];

		// loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vec3_rotate_x(transformed_vertex, g_mesh2.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, g_mesh2.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, g_mesh2.rotation.z);

			// Translate the vertex away from the camera
			transformed_vertex.z += 5; //inside monitor

			// Save transfomed vertex in the arrya of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		if (cull_method == CULL_NONE && backface_culling(transformed_vertices))
		{
			continue;
		}

		vec2_t projected_points[3];
		// Loop all three vertices tor perform projection
		for (int j = 0; j < 3; j++)
		{
			// Project the current vertex
			projected_points[j] = project(transformed_vertices[j]);

			// Scale and translate the projected points to the middle of the screen
			projected_points[j].x += (window_width / 2);
			projected_points[j].y += (window_height / 2);
		}

		triangle_t projected_triangle = {
			.points = {
				{ projected_points[0].x, projected_points[0].y },
				{ projected_points[1].x, projected_points[1].y },
				{ projected_points[2].x, projected_points[2].y },
			},
			.color = mesh_face.color,
		};

		// save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}
}

void render(void) 
{
	draw_grid();

	// loop all projected triangles and render them
	int triangle_array_length = array_length(triangles_to_render);
	for (int i = 0; i < triangle_array_length; i++)
	{
		triangle_t triangle = triangles_to_render[i];

		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
			draw_filled_triangle2(&triangle, triangle.color);

		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE)
			draw_triangle(triangle, Green); // Wireframe

		if (render_method == RENDER_WIRE_VERTEX)
		{
			draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, vertex_color); // vertex A
			draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, vertex_color); // vertex B
			draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, vertex_color); // vertex C
		}
	}

	// Clear the array of triangles to render every frame loop
	array_free(triangles_to_render);
	render_color_buffer();
	clear_color_buffer(None);
	SDL_RenderPresent(renderer);
}

// Free the memory that was dynamically allocated by the program
void free_resources(void)
{
	array_free(g_mesh2.faces);
	array_free(g_mesh2.vertices);
	// free(color_buffer);
}

int main(void)
{
	is_running = initialize_window();

	setup();

	while (is_running) 
	{
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}