// TODO : mesh.h함수 구현
#include "mesh.h"
#include "array.h"

mesh_t g_mesh =
{
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0,0,0}
};

mesh_t g_mesh2 =
{
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0,0,0}
};

vec3_t g_cube_vertices[N_CUBE_VERTICES] = 
{
    {.x = -1, .y = -1, .z = -1 },   // 1
    {.x = -1, .y =  1, .z = -1 },   // 2
    {.x =  1, .y =  1, .z = -1 },   // 3
    {.x =  1, .y = -1, .z = -1 },   // 4
    {.x =  1, .y =  1, .z =  1 },   // 5
    {.x =  1, .y = -1, .z =  1 },   // 6
    {.x = -1, .y =  1, .z =  1 },   // 7
    {.x = -1, .y = -1, .z =  1 }    // 8
};

face_t g_cube_faces[N_CUBE_FACES] = 
{
    // front
    {.a = 1, .b = 2, .c = 3 },
    {.a = 1, .b = 3, .c = 4 },
    // right
    {.a = 4, .b = 3, .c = 5 },
    {.a = 4, .b = 5, .c = 6 },
    // back
    {.a = 6, .b = 5, .c = 7 },
    {.a = 6, .b = 7, .c = 8 },
    // left
    {.a = 8, .b = 7, .c = 2 },
    {.a = 8, .b = 2, .c = 1 },
    // top
    {.a = 2, .b = 7, .c = 5 },
    {.a = 2, .b = 5, .c = 3 },
    // bottom
    {.a = 6, .b = 8, .c = 1 },
    {.a = 6, .b = 1, .c = 4 }
};

void load_cube_mesh_data(void)
{
    for (int i = 0; i < N_CUBE_VERTICES; i++)
    {
        vec3_t cube_vertex = g_cube_vertices[i];
        array_push(g_mesh.vertices, cube_vertex);
    }

    for (int i = 0; i < N_CUBE_FACES; i++)
    {
        face_t cube_face = g_cube_faces[i];
        array_push(g_mesh.faces, cube_face);
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

    while (fgets(buffer, sizeof(buffer), fr) != NULL)
    {
        char first = buffer[0];
        char second = buffer[1];
        vec3_t v = { 0, 0, 0 };
        if (first == 'v' && second == ' ')
        {
            float arr[3] = {0.f, 0.f, 0.f};
            int cnt = 0;
            char* ptr = strtok(buffer, " v");
            while (ptr != NULL)
            {
                arr[cnt++] = (float)atof(ptr);

                printf(ptr);
                ptr = strtok(NULL, " v");
            }

            v.x = arr[0]; 
            v.y = arr[1]; 
            v.z = arr[2];
            array_push(g_mesh2.vertices, v);
        }

        face_t f = { 0, 0, 0 };
        if (first == 'f' && second == ' ')
        {
            int arr[3] = {0,0,0};
            int cnt = 0;
            char* ptr = strtok(buffer, " f");
            while (ptr != NULL)
            {
                arr[cnt++] = (int)atof(ptr);
                printf(ptr);
                ptr = strtok(NULL, " f");
            }

            f.a = arr[0]; 
            f.b = arr[1]; 
            f.c = arr[2];
            array_push(g_mesh2.faces, f);
        }
        count++;
    }

    fclose(fr);
    printf("FILE %s has %lu character\n", filename, count);
}