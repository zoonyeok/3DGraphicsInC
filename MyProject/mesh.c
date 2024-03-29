// TODO : mesh.h�Լ� ����
#include "mesh.h"
#include "array.h"
#include "string.h"
#include <stdlib.h>
#include "color.h"
#include "triangle.h"

mesh_t g_mesh2 =
{
	.vertices = NULL,
	.faces = NULL,
	.rotation = { 0 ,0, 0 },
	.scale = { 1.0f, 1.0f, 1.0f },
	.translation = { 0, 0, 0},
};

vec3_t g_cube_vertices[N_CUBE_VERTICES] =
{
	{.x = -1, .y = -1, .z = -1 },   // 1
	{.x = -1, .y = 1, .z = -1 },   // 2
	{.x = 1, .y = 1, .z = -1 },   // 3
	{.x = 1, .y = -1, .z = -1 },   // 4
	{.x = 1, .y = 1, .z = 1 },   // 5
	{.x = 1, .y = -1, .z = 1 },   // 6
	{.x = -1, .y = 1, .z = 1 },   // 7
	{.x = -1, .y = -1, .z = 1 }    // 8
};

face_t g_cube_faces[N_CUBE_FACES] = 
{
	// front
	{.a = 1, .b = 2, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 1, .b = 3, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	// right
	{.a = 4, .b = 3, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 4, .b = 5, .c = 6, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	// back
	{.a = 6, .b = 5, .c = 7, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 6, .b = 7, .c = 8, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	// left
	{.a = 8, .b = 7, .c = 2, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 8, .b = 2, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	// top
	{.a = 2, .b = 7, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 2, .b = 5, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
	// bottom
	{.a = 6, .b = 8, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
	{.a = 6, .b = 1, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF }
};

void load_cube_mesh_data(void)
{
	for (int i = 0; i < N_CUBE_VERTICES; i++)
	{
		vec3_t cube_vertex = g_cube_vertices[i];
		array_push(g_mesh2.vertices, cube_vertex);
	}

	for (int i = 0; i < N_CUBE_FACES; i++)
	{
		face_t cube_face = g_cube_faces[i];
		array_push(g_mesh2.faces, cube_face);
	}
}

void load_obj_file_data(char* filename)
{
	FILE* fr = NULL;
	char ch;
	float num = 0.f;
	unsigned long count = 0;
	char buffer[256];

	fopen_s(&fr, filename, "r");
	if (fr == NULL)
	{
		printf("Can't open %s\n", filename);
		exit(EXIT_FAILURE);
	}

	tex2_t* vt_buffer = NULL;
	int vt_buffer_idx = 0;
	while (fgets(buffer, sizeof(buffer), fr) != NULL)
	{
		char* context = NULL;
		char first = buffer[0];
		char second = buffer[1];
		vec3_t v = { 0, 0, 0 };

		if (first == 'v' && second == ' ')
		{
			float arr[3] = { 0.f, 0.f, 0.f };
			int cnt = 0;
			char* ptr = strtok_s(buffer, " v", &context);
			while (ptr != NULL)
			{
				arr[cnt++] = (float)atof(ptr);
				ptr = strtok_s(NULL, " v", &context);
			}

			v.x = arr[0];
			v.y = arr[1];
			v.z = arr[2];
			array_push(g_mesh2.vertices, v);
		}

		tex2_t vt = { 0, 0 };
		if (first == 'v' && second == 't')
		{
			float vtArr[3] = { 0.0f, 0.0f, 0.0f };
			int cnt = 0;
			char* ptr = strtok_s(buffer, " vt", &context);
			while (ptr != NULL)
			{
				vtArr[cnt++] = (float)atof(ptr);
				ptr = strtok_s(NULL, " vt", &context);
			}

			vt.u = vtArr[0];
			vt.v = vtArr[1];
			array_push(vt_buffer, vt);
		}

		face_t f = { 0, 0, 0 };
		int vt_total_count = array_length(vt_buffer);

		if (strncmp(buffer, "f ", 2) == 0)
		{
			int vertex_indices[3];
			int texture_indices[3];
			int normal_indices[3];
			sscanf_s(
				buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_indices[0], &texture_indices[0], &normal_indices[0],
				&vertex_indices[1], &texture_indices[1], &normal_indices[1],
				&vertex_indices[2], &texture_indices[2], &normal_indices[2]
			);

			f.a = vertex_indices[0] - 1;
			f.b = vertex_indices[1] - 1;
			f.c = vertex_indices[2] - 1;
			if (vt_total_count != 0)
			{
				f.a_uv = vt_buffer[texture_indices[0] - 1];
				f.b_uv = vt_buffer[texture_indices[1] - 1];
				f.c_uv = vt_buffer[texture_indices[2] - 1];
			}
			f.color = White;

			array_push(g_mesh2.faces, f);
		}
		count++;
	}

	array_free(vt_buffer);
	fclose(fr);
	// printf("FILE %s has %lu character\n", filename, count);
}

void load_obj_file_data2(char* filename) {
	FILE* file;
	file = fopen(filename, "r");
	char line[1024];

	tex2_t* texcoords = NULL;

	while (fgets(line, 1024, file)) {
		// Vertex information
		if (strncmp(line, "v ", 2) == 0) {
			vec3_t vertex;
			sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			array_push(g_mesh2.vertices, vertex);
		}
		// Texture coordinate information
		if (strncmp(line, "vt ", 3) == 0) {
			tex2_t texcoord;
			sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
			array_push(texcoords, texcoord);
		}
		// Face information
		if (strncmp(line, "f ", 2) == 0) {
			int vertex_indices[3];
			int texture_indices[3];
			int normal_indices[3];
			sscanf(
				line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_indices[0], &texture_indices[0], &normal_indices[0],
				&vertex_indices[1], &texture_indices[1], &normal_indices[1],
				&vertex_indices[2], &texture_indices[2], &normal_indices[2]
			);
			face_t face = {
				.a = vertex_indices[0],
				.b = vertex_indices[1],
				.c = vertex_indices[2],
				.a_uv = texcoords[texture_indices[0] - 1],
				.b_uv = texcoords[texture_indices[1] - 1],
				.c_uv = texcoords[texture_indices[2] - 1],
				.color = 0xFFFFFFFF
			};
			array_push(g_mesh2.faces, face);
		}
	}
	array_free(texcoords);
	fclose(file);
}