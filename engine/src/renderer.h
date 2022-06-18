#ifndef RENDERER_H
#define RENDERER_H

#define BATCH_SIZE 1000

#define POS_SIZE 2
#define COLOR_SIZE 4
#define TEX_COORDS_SIZE 2
#define TEX_ID_SIZE 1

#define POS_OFFSET 0
#define COLOR_OFFSET ((POS_OFFSET + POS_SIZE) * sizeof(f32))
#define TEX_COORDS_OFFSET (COLOR_OFFSET + COLOR_SIZE * sizeof(f32))
#define TEX_ID_OFFSET (TEX_COORDS_OFFSET + TEX_COORDS_SIZE * sizeof(f32))

#define VERTEX_SIZE 9
#define VERTEX_SIZE_BYTES (VERTEX_SIZE * sizeof(f32))

struct renderer
{
    u32 VAO;
    u32 VBO;
    u32 EBO;
    float *Vertices;
    i32 QuadCount;
};

#endif
