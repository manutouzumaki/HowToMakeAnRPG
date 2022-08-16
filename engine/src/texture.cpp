global_variable texture_atlas GlobalTextureAtlas;

internal const char *GetTextureNameFromPath(const char *Path)
{
    char *Letter = (char *)Path;
    i32 Offset = 0;
    i32 BarsFound = 0;
    while(*Letter++ != '\0')
    {
        Offset++;
        if(*Letter == '/')
        {
            BarsFound++;
        }
        if(BarsFound == 4)
        {
            Offset++;
            break;
        }
    }
    return Path + Offset; 
}

internal void LoadTexturesToAtlas(const char **Path, i32 TextureCount)
{
    i32 AtlasWidth = 128;
    i32 AtlasHeight = 128;
    raw_texture *Textures = (raw_texture *)malloc(sizeof(raw_texture) * TextureCount);
    for(i32 i = 0; i < TextureCount; ++i)
    {
        Textures[i].Data = stbi_load(Path[i], &Textures[i].Width, &Textures[i].Height, &Textures[i].Channels, 0);
        AtlasWidth = Textures[i].Width > AtlasWidth ? Textures[i].Width : AtlasWidth;
        AtlasHeight = Textures[i].Height > AtlasHeight ? Textures[i].Height : AtlasHeight;
        Textures[i].Name = GetTextureNameFromPath(Path[i]); 
    }
    AtlasWidth *= 2;
    AtlasHeight *= 2;

    GlobalTextureAtlas.Data = (u32 *)malloc(AtlasWidth * AtlasHeight * sizeof(u32));
    memset(GlobalTextureAtlas.Data, 0, AtlasWidth * AtlasHeight * sizeof(u32));
    GlobalTextureAtlas.Width = AtlasWidth;
    GlobalTextureAtlas.Height = AtlasHeight;
    GlobalTextureAtlas.TextureCount = TextureCount;

    i32 OffsetX = 0;
    i32 OffsetY = 0;
    for(i32 i = 0; i < TextureCount; ++i)
    {
        u8 *RowSrc = Textures[i].Data;
        for(i32 y = 0; y < Textures[i].Height; ++y)
        {
            for(i32 x = 0; x < Textures[i].Width; ++x)
            {
                u8 R = *RowSrc++;
                u8 G = *RowSrc++;
                u8 B = *RowSrc++; 
                u8 A = 0xFF;
                u32 *Dest = GlobalTextureAtlas.Data + (OffsetY * AtlasWidth) + OffsetX;
                Dest[y * AtlasWidth + x] = A << 24 | B << 16 | G << 8 | R;
            }
        }
        // TODO: save the uvs to select the texture
        texture_info Info;
        Info.Uvs.push_back((f32)OffsetX / (f32)AtlasWidth);
        Info.Uvs.push_back((f32)OffsetY / (f32)AtlasHeight);
        Info.Uvs.push_back((f32)(Textures[i].Width + OffsetX) / (f32)AtlasWidth);
        Info.Uvs.push_back((f32)(Textures[i].Height + OffsetY) / (f32)AtlasHeight);
        Info.Width = Textures[i].Width;
        Info.Height = Textures[i].Height;
        GlobalTextureAtlas.Info[Textures[i].Name] = Info;

        if(OffsetX + Textures[i].Width <= AtlasWidth)
        {
            OffsetX += Textures[i].Width;
        }
        else
        {
            OffsetX = 0;
            OffsetY += AtlasHeight/2;
        }
    }
    free(Textures);
}

internal texture *TextureCreate(const char *Path)
{
    texture *Texture = (texture *)malloc(sizeof(texture));
    memset(Texture, 0, sizeof(texture));

    unsigned char *Data = stbi_load(Path, &Texture->Width, &Texture->Height, &Texture->Channels, 0);
    
    GLenum format = {};
    switch(Texture->Channels)
    {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: Assert(!"Format not Supported");
    }
    
    glGenTextures(1, &Texture->ID);
    glBindTexture(GL_TEXTURE_2D, Texture->ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, Texture->Width, Texture->Height, 0, format, GL_UNSIGNED_BYTE, Data);

    stbi_image_free(Data);
    return Texture;    
}

internal texture *TextureCreateFromBuffer(unsigned char *Buffer, i32 Width, i32 Height, i32 Channels)
{
    texture *Texture = (texture *)malloc(sizeof(texture));
    memset(Texture, 0, sizeof(texture));
    Texture->Width = Width;
    Texture->Height = Height;
    Texture->Channels = Channels;

    GLenum format = {};
    switch(Texture->Channels)
    {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: Assert(!"Format not Supported");
    }
    
    glGenTextures(1, &Texture->ID);
    glBindTexture(GL_TEXTURE_2D, Texture->ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glTexImage2D(GL_TEXTURE_2D, 0, format, Texture->Width, Texture->Height, 0, format, GL_UNSIGNED_BYTE, Buffer);
    return Texture;    
}



internal void TextureDestroy(texture *Texture)
{
    glDeleteTextures(1, &Texture->ID);
    free(Texture);
}

internal void TextureBind(texture *Texture) {
    glBindTexture(GL_TEXTURE_2D, Texture->ID);
}

internal void TextureUnbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

