#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2) // 6cubes faces, 2 triangles per face

extern vec3_t g_cube_vertices[N_CUBE_VERTICES];
extern face_t g_cube_faces[N_CUBE_FACES];

/// <summary>
/// Define a struct for dynamic size meshes, with array of vertices and faces
/// </summary>
typedef struct
{
	vec3_t* vertices; // dynamic aray of vertices
	face_t* faces;    // dynamic aray of faces
	vec3_t  rotation; // rotation with x, y, and z values
} mesh_t;

extern mesh_t g_mesh;

void load_cube_mesh_data(void);

#endif // !MESH_H