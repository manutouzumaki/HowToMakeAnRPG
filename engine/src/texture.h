#ifndef TEXTURE_H
#define TEXTURE_H

struct texture
{
    u32 ID;
    i32 Width;
    i32 Height;
    i32 Channels;
};

struct raw_texture
{
    unsigned char *Data;
    i32 Width;
    i32 Height;
    i32 Channels;

};

struct texture_atlas
{
    u32 *Data;
    i32 Width;
    i32 Height;
    i32 TextureCount;
    std::map<std::string, std::vector<f32>> Uvs;
};

#endif
