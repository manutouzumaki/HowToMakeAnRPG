
inline u32 BitScanForward(u32 Mask)
{
    unsigned long Shift = 0;
    _BitScanForward(&Shift, Mask);
    return (u32)Shift;
}

internal texture *TextureCreateFromBuffer(void *Buffer, i32 Width, i32 Height)
{
    texture *Texture = (texture *)malloc(sizeof(texture));
    Texture->Width = Width;
    Texture->Height = Height;
    glGenTextures(1, &Texture->ID);
    glBindTexture(GL_TEXTURE_2D, Texture->ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
    return Texture;
}

internal texture *TextureCreate(const char *Filepath)
{
    read_file_result File = Win32ReadEntireFile(Filepath);
    bmp_header *BmpHeader = (bmp_header *)File.data; 
    u8 *data = (u8 *)File.data + BmpHeader->BitmapOffset;
    texture *Texture = TextureCreateFromBuffer(data, BmpHeader->Width, BmpHeader->Height);
    free(File.data);
    return Texture;
}

internal void TextureDestroy(texture *Texture)
{
    glDeleteTextures(1, &Texture->ID);
    free(Texture);
}

internal void TextureBind(texture *Texture)
{
    glBindTexture(GL_TEXTURE_2D, Texture->ID);
}

internal void TextureUnbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

internal f32 *GenerateUVs(texture *Texture, i32 TileWidth, i32 TileHeight)
{
    f32 Width = (f32)TileWidth / (f32)Texture->Width;
    f32 Height = (f32)TileHeight / (f32)Texture->Height;
    i32 Cols = Texture->Width / TileWidth;
    i32 Rows = Texture->Height / TileHeight;

    f32 *UVs = (f32 *)malloc(Cols * Rows * 4 * sizeof(f32));

    f32 Ux = 0.0f;
    f32 Uy = 0.0f;
    f32 Vx = Width;
    f32 Vy = Height;
    
    f32 *UVs_ptr = UVs;
    for(i32 J = 0; J < Rows; ++J) {
        for(i32 I = 0; I < Cols; ++I) {
            *UVs_ptr++ = Ux;
            *UVs_ptr++ = Uy;
            *UVs_ptr++ = Vx;
            *UVs_ptr++ = Vy;

            Ux += Width;
            Vx += Width;
        }
        Ux = 0;
        Vx = Width;
        Uy += Height;
        Vy += Height;
    }
    return UVs;
}
