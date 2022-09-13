#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include "upng.h"

typedef struct
{
    float u;
    float v;
} tex2_t;

extern int texture_width;
extern int texture_height;

extern uint32_t* mesh_texture;
extern unsigned char* mesh_texture1;
extern upng_t* png_texture;

extern const uint8_t REDBRICK_TEXTURE[];

void load_png_texture_data(char* filename);

#endif