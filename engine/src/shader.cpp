
u32 CreateShader(const char *VertexFilepath, const char *FragmentFilepath)
{
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

    u32 Program = glCreateProgram();
    glAttachShader(Program, VertexID);
    glAttachShader(Program, FragmentID);
    glLinkProgram(Program);
    glGetProgramiv(Program, GL_LINK_STATUS, &Succes);
    if(!Succes)
    {
        OutputDebugString("Error Linking Shader Program\n"); 
        glGetProgramInfoLog(Program, 512, 0, InfoLog);
        OutputDebugString(InfoLog);
        OutputDebugString("\n");
    }

    glDeleteShader(VertexID);
    glDeleteShader(FragmentID);

    free(VertexResult.data);
    free(FragmentResult.data);

    return Program;
}

internal void BindShader(u32 Shader)
{
    glUseProgram(Shader);
}

internal void UnbindShader()
{
    glUseProgram(0);
}

internal void UpdateVec3f(u32 Shader, const char *VarName, glm::vec3 Vector)
{
    i32 VarLoc = glGetUniformLocation(Shader, VarName);
    BindShader(Shader);
    glUniform3fv(VarLoc, 1, &Vector[0]); 
}

internal void UpdateMat4f(u32 Shader, const char *VarName, glm::mat4 Matrix)
{
    i32 VarLoc = glGetUniformLocation(Shader, VarName);
    BindShader(Shader);
    glUniformMatrix4fv(VarLoc, 1, false, &Matrix[0][0]);
}

internal void UpdateInt(u32 Shader, const char *VarName, u32 Value)
{ 
    i32 VarLoc = glGetUniformLocation(Shader, VarName);
    BindShader(Shader);
    glUniform1i(VarLoc, Value);
}

internal void UpdateIntArray(u32 Shader, const char *VarName, i32 Size, i32 *Array)
{
    i32 VarLoc = glGetUniformLocation(Shader, VarName);
    BindShader(Shader);
    glUniform1iv(VarLoc, Size, Array);
}


