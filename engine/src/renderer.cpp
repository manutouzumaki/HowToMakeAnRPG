internal void LoadElementIndices(u32 *Indices, i32 Index)
{
    i32 OffsetArrayIndex = 6 * Index;
    i32 Offset = 4 * Index; 
    Indices[OffsetArrayIndex] = Offset + 3;
    Indices[OffsetArrayIndex + 1] = Offset + 2;
    Indices[OffsetArrayIndex + 2] = Offset;
    Indices[OffsetArrayIndex + 3] = Offset;
    Indices[OffsetArrayIndex + 4] = Offset + 2;
    Indices[OffsetArrayIndex + 5] = Offset + 1;
}

internal void GenerateIndices(u32 *Indices)
{
    for(i32 Index = 0; Index < BATCH_SIZE; ++Index)
    {
        LoadElementIndices(Indices, Index);        
    }
}

internal i32 RendererCreate(lua_State *LuaState)
{
    renderer *Renderer = (renderer *)malloc(sizeof(renderer));
    memset(Renderer, 0, sizeof(renderer));

    Renderer->Position = glm::vec3(0, 0, 20);
    Renderer->Front = glm::vec3(Renderer->Position.x, Renderer->Position.y, -1);
    Renderer->Up = glm::vec3(0, 1, 0);

    glGenVertexArrays(1, &Renderer->VAO);
    glBindVertexArray(Renderer->VAO);

    Renderer->Vertices = (f32 *)malloc(BATCH_SIZE * (VERTEX_SIZE_BYTES * 4));

    glGenBuffers(1, &Renderer->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Renderer->VBO);
    glBufferData(GL_ARRAY_BUFFER, BATCH_SIZE * (VERTEX_SIZE_BYTES * 4), 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void *)POS_OFFSET);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void *)COLOR_OFFSET);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, TEX_COORDS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void *)TEX_COORDS_OFFSET);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, TEX_ID_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void *)TEX_ID_OFFSET);
    glEnableVertexAttribArray(3);
    
    u32 *Indices = (u32 *)malloc(BATCH_SIZE * (sizeof(u32) * 6));
    GenerateIndices(Indices);

    glGenBuffers(1, &Renderer->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Renderer->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, BATCH_SIZE * (sizeof(u32) * 6), Indices, GL_STATIC_DRAW); 

    free(Indices);
    
    u64 Addres = (u64)Renderer;
    lua_pushinteger(LuaState, Addres);
    
    return 1;    
}

internal i32 RendererTranslate(lua_State *LuaState)
{
    renderer *Renderer = (renderer *)lua_tointeger(LuaState, -3);
    i32 X = (i32)lua_tonumber(LuaState, -2);
    i32 Y = (i32)lua_tonumber(LuaState, -1);

    Renderer->Position.x = (f32)X;
    Renderer->Position.y = (f32)Y;
    Renderer->Front = glm::vec3(Renderer->Position.x, Renderer->Position.y, -1);
    glm::mat4 ViewMat = glm::lookAt(Renderer->Position,
                                    Renderer->Front,
                                    Renderer->Up);
    UpdateMat4f(Renderer->CurrentShader, "uView", ViewMat);
    return 0;
}

internal void RendererDestroy(renderer *Renderer)
{
    glDeleteVertexArrays(1, &Renderer->VAO);
    glDeleteBuffers(1, &Renderer->VBO);
    glDeleteBuffers(1, &Renderer->EBO);
    free(Renderer->Vertices);
}

internal void FlushRenderQueue(renderer *Renderer)
{
    Renderer->CurrentDrawCallCount++;
    glBindBuffer(GL_ARRAY_BUFFER, Renderer->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (Renderer->QuadCount * 4) * VERTEX_SIZE_BYTES, Renderer->Vertices); 
    glDrawElements(GL_TRIANGLES, Renderer->QuadCount * 6, GL_UNSIGNED_INT, 0);
    Renderer->QuadCount = 0;
}

internal void RenderBegin(renderer *Renderer, shader *Shader)
{
    ShaderBind(Shader);
    glBindVertexArray(Renderer->VAO);
}

internal void RenderEnd(renderer *Renderer)
{
    glBindVertexArray(Renderer->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Renderer->VBO);
    FlushRenderQueue(Renderer);
}

internal void AddQuadToRenderQueue(renderer *Renderer, f32 XPos, f32 YPos, f32 Width, f32 Height, f32 Angle, f32 *Uvs)
{
    if(Renderer->QuadCount >= BATCH_SIZE)
    {
        FlushRenderQueue(Renderer);
    }
    f32 *Vertices = Renderer->Vertices + (Renderer->QuadCount * VERTEX_SIZE * 4);
    f32 UvsArray[8] = {
        Uvs[2], Uvs[3],
        Uvs[2], Uvs[1],
        Uvs[0], Uvs[1],
        Uvs[0], Uvs[3]
    };
    f32 *UvsPtr = UvsArray;
    f32 X = 0.5f;
    f32 Y = 0.5f; 
    for(i32 Index = 0; Index < 4; ++Index)
    {
        if(Index == 1)
            Y = -0.5f;   
        else if(Index == 2)
            X = -0.5f;
        else if(Index == 3)
            Y = 0.5f;
        glm::vec2 CurrentP = glm::vec2(X, Y);
        if(Angle > 0.0f)
        {
            CurrentP = glm::rotate(CurrentP, Angle);
        }
        CurrentP.x *= Width;
        CurrentP.y *= Height;
        CurrentP.x += XPos;
        CurrentP.y += YPos;
        *Vertices++ = CurrentP.x;
        *Vertices++ = CurrentP.y;
        *Vertices++ = 1.0f;
        *Vertices++ = 1.0f;
        *Vertices++ = 1.0f;
        *Vertices++ = 1.0f;
        *Vertices++ = *UvsPtr++;
        *Vertices++ = *UvsPtr++;
        *Vertices++ = 1.0f;
    }
    Renderer->QuadCount++; 
}

internal void AddQuadToRenderQueue(renderer *Renderer, f32 XPos, f32 YPos, f32 Width, f32 Height, f32 Angle, f32 R, f32 G, f32 B, f32 A)
{
    if(Renderer->QuadCount >= BATCH_SIZE)
    {
        FlushRenderQueue(Renderer);
    }
    f32 *Vertices = Renderer->Vertices + (Renderer->QuadCount * VERTEX_SIZE * 4);
    f32 X = 0.5f;
    f32 Y = 0.5f; 
    for(i32 Index = 0; Index < 4; ++Index)
    {
        if(Index == 1)
            Y = -0.5f;   
        else if(Index == 2)
            X = -0.5f;
        else if(Index == 3)
            Y = 0.5f;
        glm::vec2 CurrentP = glm::vec2(X, Y);
        if(Angle > 0.0f)
        {
            CurrentP = glm::rotate(CurrentP, Angle);
        }
        CurrentP.x *= Width;
        CurrentP.y *= Height;
        CurrentP.x += XPos;
        CurrentP.y += YPos;
        *Vertices++ = CurrentP.x;
        *Vertices++ = CurrentP.y;
        *Vertices++ = R;
        *Vertices++ = G;
        *Vertices++ = B;
        *Vertices++ = A;
        *Vertices++ = 0.0f;
        *Vertices++ = 0.0f;
        *Vertices++ = 0.0f;
    }
    Renderer->QuadCount++; 
}

internal i32 DrawBox2d(lua_State *LuaState)
{
    renderer *Renderer = (renderer *)lua_tointeger(LuaState, -6);
    i32 X = (i32)lua_tonumber(LuaState, -5);
    i32 Y = (i32)lua_tonumber(LuaState, -4);
    i32 W = (i32)lua_tonumber(LuaState, -3);
    i32 H = (i32)lua_tonumber(LuaState, -2);
    lua_getfield(LuaState, -1, "r");
    f32 R = (f32)lua_tonumber(LuaState, -1);
    lua_getfield(LuaState, -2, "g");
    f32 G = (f32)lua_tonumber(LuaState, -1);
    lua_getfield(LuaState, -3, "b");
    f32 B = (f32)lua_tonumber(LuaState, -1);
    AddQuadToRenderQueue(Renderer, (f32)X, (f32)Y, (f32)W, (f32)H, (f32)glm::radians(0.0f), R, G, B, 1);
    return 0;
}

internal i32 DrawTexture(lua_State *LuaState)
{
    renderer *Renderer = (renderer *)lua_tointeger(LuaState, -6);
    i32 X = (i32)lua_tonumber(LuaState, -5);
    i32 Y = (i32)lua_tonumber(LuaState, -4);
    i32 W = (i32)lua_tonumber(LuaState, -3);
    i32 H = (i32)lua_tonumber(LuaState, -2);
    const char *Name = (const char *)lua_tostring(LuaState, -1);
    texture_info TextureInfo = GlobalTextureAtlas.Info[Name]; 
    AddQuadToRenderQueue(Renderer, (f32)X, (f32)Y, (f32)W, (f32)H, (f32)glm::radians(0.0f), &TextureInfo.Uvs[0]);
    return 0;
}

internal i32 DrawSprite(lua_State *LuaState)
{
    renderer *Renderer = (renderer *)lua_tointeger(LuaState, -10);
    i32 X = (i32)lua_tonumber(LuaState, -9);
    i32 Y = (i32)lua_tonumber(LuaState, -8);
    i32 W = (i32)lua_tonumber(LuaState, -7);
    i32 H = (i32)lua_tonumber(LuaState, -6);
    const char *Name = (const char *)lua_tostring(LuaState, -5);
    f32 RelU0 = (f32)lua_tonumber(LuaState, -4); 
    f32 RelV0 = (f32)lua_tonumber(LuaState, -3); 
    f32 RelU1 = (f32)lua_tonumber(LuaState, -2); 
    f32 RelV1 = (f32)lua_tonumber(LuaState, -1); 
    texture_info TextureInfo = GlobalTextureAtlas.Info[Name];
    
    f32 AbsU0 = TextureInfo.Uvs[0];
    f32 AbsV0 = TextureInfo.Uvs[1];
    f32 AbsU1 = TextureInfo.Uvs[2];
    f32 AbsV1 = TextureInfo.Uvs[3];

    f32 ULength = AbsU1 - AbsU0;
    f32 VLength = AbsV1 - AbsV0;

    f32 U0Offset = ULength * RelU0;
    f32 V0Offset = VLength * RelV0;
    f32 U1Offset = ULength * RelU1;
    f32 V1Offset = VLength * RelV1;

    f32 Uvs[4] = {
        AbsU0 + U0Offset, VLength - (AbsV0 + V1Offset), 
        AbsU0 + U1Offset, VLength - (AbsV0 + V0Offset)
    };
    AddQuadToRenderQueue(Renderer, (f32)X, (f32)Y, (f32)W, (f32)H, (f32)glm::radians(0.0f), Uvs);
    return 0;
}
