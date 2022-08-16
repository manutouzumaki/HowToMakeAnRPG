shader *ShaderCreate(const char *VertexFilepath, const char *FragmentFilepath)
{
    shader *Shader = (shader *)malloc(sizeof(shader));
    read_file_result VertexResult = Win32ReadEntireFile(VertexFilepath);
    read_file_result FragmentResult = Win32ReadEntireFile(FragmentFilepath);

    const char * VertexSrc = (const char *)VertexResult.data;
    const char * FragmentSrc = (const char *)FragmentResult.data;

    i32 Succes;
    char InfoLog[512];

    i32 VertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexID, 1, &VertexSrc, 0);
    glCompileShader(VertexID);
    glGetShaderiv(VertexID, GL_COMPILE_STATUS, &Succes);
    if(!Succes)
    {
        OutputDebugString("Error Compiling: "); 
        OutputDebugString(VertexFilepath); 
        OutputDebugString("\n");
        glGetShaderInfoLog(VertexID, 512, 0, InfoLog);
        OutputDebugString(InfoLog);
        OutputDebugString("\n");
    }

    i32 FragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentID, 1, &FragmentSrc, 0);
    glCompileShader(FragmentID);
    glGetShaderiv(FragmentID, GL_COMPILE_STATUS, &Succes);
    if(!Succes)
    {
        OutputDebugString("Error Compiling: "); 
        OutputDebugString(FragmentFilepath); 
        OutputDebugString("\n");
        glGetShaderInfoLog(FragmentID, 512, 0, InfoLog);
        OutputDebugString(InfoLog);
        OutputDebugString("\n");
    }

    Shader->ID = glCreateProgram();
    glAttachShader(Shader->ID, VertexID);
    glAttachShader(Shader->ID, FragmentID);
    glLinkProgram(Shader->ID);
    glGetProgramiv(Shader->ID, GL_LINK_STATUS, &Succes);
    if(!Succes)
    {
        OutputDebugString("Error Linking Shader Program\n"); 
        glGetProgramInfoLog(Shader->ID, 512, 0, InfoLog);
        OutputDebugString(InfoLog);
        OutputDebugString("\n");
    }

    glDeleteShader(VertexID);
    glDeleteShader(FragmentID);

    free(VertexResult.data);
    free(FragmentResult.data);

    return Shader;
}

internal void ShaderDestroy(shader *Shader)
{
    glDeleteProgram(Shader->ID);
    free(Shader);
}

internal void ShaderBind(shader *Shader)
{
    glUseProgram(Shader->ID);
}

internal void ShaderUnbind()
{
    glUseProgram(0);
}


internal void UpdateVec3f(shader *Shader, const char *VarName, glm::vec3 Vector)
{
    i32 VarLoc = glGetUniformLocation(Shader->ID, VarName);
    ShaderBind(Shader);
    glUniform3fv(VarLoc, 1, &Vector[0]); 
}

internal void UpdateMat4f(shader *Shader, const char *VarName, glm::mat4 Matrix)
{
    i32 VarLoc = glGetUniformLocation(Shader->ID, VarName);
    ShaderBind(Shader);
    glUniformMatrix4fv(VarLoc, 1, false, &Matrix[0][0]);
}

internal void UpdateInt(shader *Shader, const char *VarName, u32 Value)
{ 
    i32 VarLoc = glGetUniformLocation(Shader->ID, VarName);
    ShaderBind(Shader);
    glUniform1i(VarLoc, Value);
}

internal void UpdateIntArray(shader *Shader, const char *VarName, i32 Size, i32 *Array)
{
    i32 VarLoc = glGetUniformLocation(Shader->ID, VarName);
    ShaderBind(Shader);
    glUniform1iv(VarLoc, Size, Array);
}


