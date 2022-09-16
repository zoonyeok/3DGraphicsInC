#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "color.h"
#include "triangle.h"
#include "texture.h"
#include "matrix.h"
#include "light.h"
#include "upng.h"
#include "camera.h"

// Array of triangle that should be rendered frame by frame
#define MAX_TRIANGLES 10000
triangle_t triangles_to_render[MAX_TRIANGLES];
int num_triangles_to_render = 0;

// Global variables for execution status and game loop
mat4_t projection_matrix;
mat4_t view_matrix;
mat4_t world_matrix;

uint32_t vertex_color = Yellow;
bool is_running = false;
uint32_t previous_frame_time = 0;
float delta_time = 0;

void setup(void)
{
	render_method = RENDER_FILL_TRIANGLE;
	cull_method = CULL_BACKFACE;

	// Allocate the required memory in bytes to hold the color and z buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
	z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);
	
	// Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	//Initialize the perpective projection matrix
	float fov = M_PI / 3.0f; // 180/3 or 60deg or M_PI
	float aspect = (float)window_height / (float)window_width;
	float znear = 0.1f;
	float zfar = 100.0f;
	projection_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

	// load_cube_mesh_data();
	load_obj_file_data2("..\\assets\\f22.obj");

	// load the texture information from an external PNG file
	load_png_texture_data("..\\assets\\f22.png");
}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);
	uint8_t* keystate = SDL_GetKeyboardState(NULL);
	// up: move up
	if (keystate[SDL_SCANCODE_UP]) {
		g_camera.position.y += 3.0 * delta_time;
	}
	// down: move down
	if (keystate[SDL_SCANCODE_DOWN]) {
		g_camera.position.y -= 3.0 * delta_time;
	}
	// A: rotate left
	if (keystate[SDL_SCANCODE_A]) {
		g_camera.yaw_angle -= 1.0 * delta_time;
	}
	// D: rotate right
	if (keystate[SDL_SCANCODE_D]) {
		g_camera.yaw_angle += 1.0 * delta_time;
	}
	// W :
	if (keystate[SDL_SCANCODE_W])
	{
		g_camera.forward_velocity = vec3_mul(g_camera.direction, 5.0f * delta_time);
		g_camera.position = vec3_add(g_camera.position, g_camera.forward_velocity);
	}
	// S : 
	if (keystate[SDL_SCANCODE_S])
	{
		g_camera.forward_velocity = vec3_mul(g_camera.direction, 5.0f * delta_time);
		g_camera.position = vec3_sub(g_camera.position, g_camera.forward_velocity);
	}

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
		if (event.key.keysym.sym == SDLK_5)
			render_method = RENDER_TEXTURED;
		if (event.key.keysym.sym == SDLK_6)
			render_method = RENDER_TEXTURED_WIRE;
		if (event.key.keysym.sym == SDLK_c)
			cull_method = CULL_BACKFACE;
		if (event.key.keysym.sym == SDLK_v)
			cull_method = CULL_NONE;
		break;
	}
}

void update(void)
{
	// Wait some time until the reach the target frame time in milliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
	// Only delay execution if we are running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(time_to_wait);
	}

	// Get a delta time factor converted to seconds to be used to update our game objects
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;

	previous_frame_time = SDL_GetTicks();

	// Initialize the counter of triangles to render for the current frame
	num_triangles_to_render = 0;

	// Change the mesh scale, rotation, and translation values per animation frame
	g_mesh2.rotation.x += 0.0 * delta_time;
	g_mesh2.rotation.y += 0.0 * delta_time;
	g_mesh2.rotation.z += 0.0 * delta_time;
	g_mesh2.translation.z = 4.0;

	// Create the view matrix looking at a hardcoded target point
	// TODO : compute the new camera rotation and translation fo the FPS camera movment
	vec3_t up_direction = { 0, 1, 0 };
	
	// Initialize the target looking at the positive z-axis
	vec3_t target = { 0, 0, 1 };
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(g_camera.yaw_angle);
	g_camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));

	// Offset the camera position in the direction where the camera is pointing at
	target = vec3_add(g_camera.position, g_camera.direction);

	view_matrix = mat4_look_at(g_camera.position, target, up_direction);

	mat4_t scale_matrix = mat4_make_scale(g_mesh2.scale.x, g_mesh2.scale.y, g_mesh2.scale.z);
	mat4_t translation_matrix = mat4_make_translation(g_mesh2.translation.x, g_mesh2.translation.y, g_mesh2.translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(g_mesh2.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(g_mesh2.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(g_mesh2.rotation.z);

	int num_faces = array_length(g_mesh2.faces);
	// 모든 triangle face 순회
	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = g_mesh2.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = g_mesh2.vertices[mesh_face.a - 1];
		face_vertices[1] = g_mesh2.vertices[mesh_face.b - 1];
		face_vertices[2] = g_mesh2.vertices[mesh_face.c - 1];

		vec4_t transformed_vertices[3];

		// loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			// Create a World Matrix SRT
			world_matrix = mat4_identity();

			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

			// Multiply the world matirx by the original vector						
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

			// Multiply the view matrix byt the vector to transform the scene to camera space
			transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

			// Save transfomed vertex in the arrya of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		// BackFace culling
		vec3_t normal = make_normal(transformed_vertices);
		if (cull_method == CULL_BACKFACE && backface_culling(transformed_vertices, normal))
		{
			continue;
		}

		vec4_t projected_points[3];

		// Loop all three vertices tor perform projection
		for (int j = 0; j < 3; j++)
		{
			// Project the current vertex
			projected_points[j] = mat4_mul_vec4_project(projection_matrix, transformed_vertices[j]);

			projected_points[j].y *= -1;

			//Scale into the view
			projected_points[j].x *= (window_width / 2.0f);
			projected_points[j].y *= (window_height / 2.0f);

			// Translate the projected points to the middle of the screen
			projected_points[j].x += (window_width / 2.0f);
			projected_points[j].y += (window_height / 2.0f);
		}

		// Calculate the triangle color based on the light angle
		// float light_intensity = light_intensity_factor(normal);
		float light_intensity_factor = -vec3_dot(normal, g_light.direction);
		uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

		triangle_t projected_triangle = 
		{
			.points = {
				{ projected_points[0].x, projected_points[0].y , projected_points[0].z, projected_points[0].w },
				{ projected_points[1].x, projected_points[1].y , projected_points[1].z, projected_points[1].w },
				{ projected_points[2].x, projected_points[2].y , projected_points[2].z, projected_points[2].w },
			},
			.texcoords = 
			{
				{ mesh_face.a_uv.u, mesh_face.a_uv.v },
				{ mesh_face.b_uv.u, mesh_face.b_uv.v },
				{ mesh_face.c_uv.u, mesh_face.c_uv.v }
			},
			.color = triangle_color
		};

		// save the projected triangle in the array of triangles to render
		if (num_triangles_to_render < MAX_TRIANGLES)
			triangles_to_render[num_triangles_to_render++] = projected_triangle;
	}
}

void render(void)
{
	draw_grid();

	// loop all projected triangles and render them
	for (int i = 0; i < num_triangles_to_render; i++)
	{
		triangle_t triangle = triangles_to_render[i];

		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
		{
			draw_filled_triangle_z(&triangle, triangle.color);
		}
			
		// Draw textured triangle 
		if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE)
		{
			draw_textured_triangle_z(&triangle, mesh_texture); // mesh_texture
		}

		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX
			|| render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE)
		{
			draw_triangle_z(&triangle, Green); // Wireframe
		}

		if (render_method == RENDER_WIRE_VERTEX)
		{
			draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, vertex_color); // vertex A
			draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, vertex_color); // vertex B
			draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, vertex_color); // vertex C
		}
	}

	// Clear the array of triangles to render every frame loop
	render_color_buffer();
	clear_color_buffer(None);
	clear_z_buffer();
	SDL_RenderPresent(renderer);
}

// Free the memory that was dynamically allocated by the program
void free_resources(void)
{
	free(color_buffer);
	free(z_buffer);
	upng_free(png_texture);
	array_free(g_mesh2.faces);
	array_free(g_mesh2.vertices);
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