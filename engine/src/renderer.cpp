internal void LoadElementIndices(u32 *Indices, i32 Index)
{
    int OffsetArrayIndex = 6 * Index;
    int Offset = 4 * Index; 
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

internal renderer CreateRenderer()
{
    renderer Renderer = {};
    
    glGenVertexArrays(1, &Renderer.VAO);
    glBindVertexArray(Renderer.VAO);

    Renderer.Vertices = (float *)malloc(BATCH_SIZE * (VERTEX_SIZE_BYTES * 4));

    glGenBuffers(1, &Renderer.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Renderer.VBO);
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

    glGenBuffers(1, &Renderer.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Renderer.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, BATCH_SIZE * (sizeof(u32) * 6), Indices, GL_STATIC_DRAW); 

    free(Indices);

    return Renderer;
    
}

internal void ShutdownRenderer(renderer *Renderer)
{
    glDeleteVertexArrays(1, &Renderer->VAO);
    glDeleteBuffers(1, &Renderer->VBO);
    glDeleteBuffers(1, &Renderer->EBO);
    free(Renderer->Vertices);
}

internal void FlushRenderQueue(renderer *Renderer)
{
    glBindVertexArray(Renderer->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Renderer->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Renderer->QuadCount * (VERTEX_SIZE_BYTES * 4), Renderer->Vertices); 

    // TODO: Render Scene...
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glDrawElements(GL_TRIANGLES, (Renderer->QuadCount * 4) * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindVertexArray(0);

    Renderer->QuadCount = 0;
}

internal void AddQuadToRenderQueue(renderer *Renderer, f32 XPos, f32 YPos, f32 Width, f32 Height, f32 Angle, f32 *Uvs)
{
    glm::mat4 TranslationMat = glm::translate(glm::mat4(1.0f), glm::vec3(XPos, YPos, 0.0f));
    glm::mat4 RotationMat = glm::rotate(glm::mat4(1.0f), Angle, glm::vec3(0, 0, 1));
    glm::mat4 ScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(Width, Height, 1.0f));
    glm::mat4 ModelMat = TranslationMat * RotationMat * ScaleMat;

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
        glm::vec4 CurrentP = ModelMat * glm::vec4(X, Y, 0, 1);
        *Vertices++ = CurrentP.x;
        *Vertices++ = CurrentP.y;
        *Vertices++ = 1.0f;
        *Vertices++ = 1.0f;
        *Vertices++ = 1.0f;
        *Vertices++ = 1.0f;
        *Vertices++ = *Uvs++;
        *Vertices++ = *Uvs++;
        *Vertices++ = 0.0f;
    }
    Renderer->QuadCount++; 
}

internal void AddQuadToRenderQueue(renderer *Renderer, f32 XPos, f32 YPos, f32 Width, f32 Height, f32 Angle, f32 R, f32 G, f32 B, f32 A)
{
    glm::mat4 TranslationMat = glm::translate(glm::mat4(1.0f), glm::vec3(XPos, YPos, 0.0f));
    glm::mat4 RotationMat = glm::rotate(glm::mat4(1.0f), Angle, glm::vec3(0, 0, 1));
    glm::mat4 ScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(Width, Height, 1.0f));
    glm::mat4 ModelMat = TranslationMat * RotationMat * ScaleMat;

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
        glm::vec4 CurrentP = ModelMat * glm::vec4(X, Y, 0, 1);
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
