#include "includes.h"

global_variable bool GlobalRunning;
global_variable lua_State *GlobalLuaState;
global_variable i32 WindowWidth; 
global_variable i32 WindowHeight;

global_variable const char **GlobalTextureNames;
global_variable i32 GlobalTextureCount;

LRESULT CALLBACK WindowProc(HWND Window, u32 Msg, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = {};
    switch(Msg)
    {
        case WM_CLOSE:
        {
            GlobalRunning = false;
        }break;
        case WM_DESTROY:
        {
            GlobalRunning = false;
        }break;
        default:
        {
            Result = DefWindowProcA(Window, Msg, WParam, LParam);
        }break;
    }
    return Result;
}

internal void Win32ProcessMessages(input *CurrInput, input *LastInput)
{
    for(i32 Index = 0; Index < ArrayCount(CurrInput->Keys); ++Index)
    {
        CurrInput->Keys[Index].WasDown = false;
    }
    for(i32 Index = 0; Index < ArrayCount(CurrInput->Buttons); ++Index)
    {
        CurrInput->Buttons[Index].WasDown = false;
    }
    for(i32 Index = 0; Index < ArrayCount(CurrInput->MouseButtons); ++Index)
    {
        CurrInput->MouseButtons[Index].WasDown = false;
    }

    MSG Msg = {};
    while(PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
    {
        switch(Msg.message)
        {
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            { 
                b8 WasDown = ((Msg.lParam & (1 << 30)) != 0);
                b8 IsDown = ((Msg.lParam & (1 << 31)) == 0);
                if(IsDown != WasDown)
                {
                    DWORD VkCode = (DWORD)Msg.wParam;
                    CurrInput->Keys[VkCode].IsDown = IsDown;
                }
            }break;
            
            case WM_MOUSEMOVE:
            {
                CurrInput->MouseX = (i32)GET_X_LPARAM(Msg.lParam); 
                CurrInput->MouseY = (i32)GET_Y_LPARAM(Msg.lParam); 
            }break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            {
                CurrInput->MouseLeft.IsDown = ((Msg.wParam & MK_LBUTTON) != 0);
                CurrInput->MouseMiddle.IsDown = ((Msg.wParam & MK_MBUTTON) != 0);
                CurrInput->MouseRight.IsDown = ((Msg.wParam & MK_RBUTTON) != 0);
            }break;
            default:
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }break;
        }        
    }
    
    XINPUT_STATE State = {};
    if(XInputGetState(0, &State) == ERROR_SUCCESS)
    {
        XINPUT_GAMEPAD *Pad = &State.Gamepad;
        for(i32 i = 0; i < ArrayCount(CurrInput->Buttons); ++i)
        {
            CurrInput->Buttons[i].IsDown = Pad->wButtons & XInputButtons[i];
        }
        CurrInput->LeftStickX = Win32ProcessXInputStick(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        CurrInput->LeftStickY = Win32ProcessXInputStick(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        CurrInput->RightStickX = Win32ProcessXInputStick(Pad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        CurrInput->RightStickY = Win32ProcessXInputStick(Pad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }
    else
    {
        for(i32 i = 0; i < ArrayCount(CurrInput->Buttons); ++i)
        {
            CurrInput->Buttons[i].IsDown = false;
        }
        CurrInput->LeftStickX = 0.0f; 
        CurrInput->LeftStickY = 0.0f;
        CurrInput->RightStickX = 0.0f;
        CurrInput->RightStickY = 0.0f;
    }
    
    for(i32 Index = 0; Index < ArrayCount(CurrInput->Keys); ++Index)
    {   
        if(LastInput->Keys[Index].IsDown)
        {
            CurrInput->Keys[Index].WasDown = true; 
        }
    }
    for(i32 Index = 0; Index < ArrayCount(CurrInput->Buttons); ++Index)
    {   
        if(LastInput->Buttons[Index].IsDown)
        {
            CurrInput->Buttons[Index].WasDown = true; 
        }
    }
    for(i32 Index = 0; Index < ArrayCount(CurrInput->MouseButtons); ++Index)
    {   
        if(LastInput->MouseButtons[Index].IsDown)
        {
            CurrInput->MouseButtons[Index].WasDown = true; 
        }
    }

}

internal void Win32InitializeOpenGLContext(HDC DeviceContext, i32 width, i32 height)
{
    PIXELFORMATDESCRIPTOR PixelFormat = {};
    PixelFormat.nSize = sizeof(PixelFormat);
    PixelFormat.nVersion = 1;
    PixelFormat.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    PixelFormat.iPixelType = PFD_TYPE_RGBA;
    PixelFormat.cColorBits = 32;
    PixelFormat.cDepthBits = 24;
    PixelFormat.cStencilBits = 8;
    PixelFormat.iLayerType = PFD_MAIN_PLANE;

    i32 WindowPixelFormat = ChoosePixelFormat(DeviceContext, &PixelFormat);
    SetPixelFormat(DeviceContext, WindowPixelFormat, &PixelFormat);

    HGLRC TempOpenglContext = wglCreateContext(DeviceContext);
    wglMakeCurrent(DeviceContext, TempOpenglContext);
    
    i32 Attributes[] = 
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 
        0
    };
    
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    if(wglCreateContextAttribsARB)
    {
        HGLRC OpenglContext = wglCreateContextAttribsARB(DeviceContext, 0, Attributes);
        wglMakeCurrent(0, 0);
        wglDeleteContext(TempOpenglContext); 
        wglMakeCurrent(DeviceContext, OpenglContext);
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 
                    (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        if(wglSwapIntervalEXT)
        {
            wglSwapIntervalEXT(1);
        }
    
        if(!gladLoadGL())
        {
            OutputDebugString("Error: glad failed Initialize\n");
        }
    }
    glViewport(0, 0, width, height);
}

internal HWND Win32InitializeWindow(HINSTANCE Instance, i32 X, i32 Y, i32 Width, i32 Height, const char *Name)
{
    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor( 0, IDC_ARROW );
    WindowClass.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    WindowClass.lpszMenuName = 0;
    WindowClass.lpszClassName = Name;

    RegisterClassEx(&WindowClass);

    RECT Rect = {X, Y, Width, Height};
    AdjustWindowRect( &Rect, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, false );

    HWND Window = CreateWindowA(Name, Name,
                                WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                Rect.right - Rect.left,
                                Rect.bottom - Rect.top,
                                0, 0, Instance, 0);
    return Window;
}

i32 StringLength(const char *String)
{
    i32 Count = 0;
    char *Letter = (char *)String;
    while(*Letter++ != '\0')
    {
        ++Count;
    }
    return Count;
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
        AbsU0 + U0Offset, AbsV0 + V0Offset, 
        AbsU0 + U1Offset, AbsV0 + V1Offset 
    };

    AddQuadToRenderQueue(Renderer, (f32)X, (f32)Y, (f32)W, (f32)H, (f32)glm::radians(0.0f), Uvs);
    return 0;
}

internal i32 GetTextureInfo(lua_State *LuaState)
{
    const char *Name = (const char *)lua_tostring(LuaState, -1);
    texture_info Info = GlobalTextureAtlas.Info[Name];
    lua_pushinteger(LuaState, Info.Width);
    lua_pushinteger(LuaState, Info.Height);
    return 2;
}

internal i32 GetDisplayInfo(lua_State *LuaState)
{
    lua_pushinteger(LuaState, WindowWidth);
    lua_pushinteger(LuaState, WindowHeight);
    return 2;
}

internal i32 SetStringArray(lua_State *LuaState)
{
    lua_len(LuaState, -1);
    i32 TextureCount = (i32)lua_tonumber(LuaState, -1);
    GlobalTextureCount = TextureCount;
    GlobalTextureNames = (const char **)malloc(sizeof(const char *) * TextureCount);
    memset(GlobalTextureNames, 0, sizeof(const char *) * TextureCount);
    for(i32 i = 1; i <= TextureCount; ++i)
    {
        lua_pushinteger(LuaState, i);
        lua_gettable(LuaState, 1);
        const char *SrcPath = (const char *)lua_tostring(LuaState, -1);
        i32 PathLength = StringLength(SrcPath);
        char *DstPath = (char *)malloc(sizeof(char) * PathLength + 1);
        DstPath[PathLength] = '\0';
        memcpy(DstPath, SrcPath, sizeof(char) * PathLength);
        GlobalTextureNames[i - 1] = DstPath;
    }
    return 0;
}

internal i32 Include(lua_State *LuaState)
{
    const char *Path = "../../game/";
    const char *File = (const char *)lua_tostring(LuaState, -1);
    i32 PathLength = StringLength(Path);
    i32 FileLength = StringLength(File);
    
    char *FilePath = (char *)malloc(sizeof(char) * PathLength + FileLength + 1);
    FilePath[PathLength + FileLength] = '\0';
    memcpy(FilePath, Path, sizeof(char) * PathLength);
    memcpy(FilePath + PathLength, File, sizeof(char) * FileLength);
    
    if(luaL_dofile(GlobalLuaState, FilePath) != LUA_OK) 
    {
        luaL_error(GlobalLuaState, "Error INCLUDING file: %s\n", lua_tostring(GlobalLuaState, -1));
        return 0;
    }

    printf("Include: %s\n", File);

    free(FilePath);
    return 0;
}

/* THE ENTRY POINT OF THE ENGINE */
int main()
{
    HINSTANCE Instance = GetModuleHandle(0);

    // TODO: load C-API test...
    GlobalLuaState = luaL_newstate();
    luaL_openlibs(GlobalLuaState);

    lua_pushcfunction(GlobalLuaState, RendererCreate);
    lua_setglobal(GlobalLuaState, "RendererCreate");

    lua_pushcfunction(GlobalLuaState, DrawBox2d);
    lua_setglobal(GlobalLuaState, "DrawBox2d");

    lua_pushcfunction(GlobalLuaState, DrawTexture);
    lua_setglobal(GlobalLuaState, "DrawTexture");

    lua_pushcfunction(GlobalLuaState, DrawSprite);
    lua_setglobal(GlobalLuaState, "DrawSprite");
   
    lua_pushcfunction(GlobalLuaState, GetTextureInfo);
    lua_setglobal(GlobalLuaState, "GetTextureInfo");

    lua_pushcfunction(GlobalLuaState, GetDisplayInfo);
    lua_setglobal(GlobalLuaState, "GetDisplayInfo");

    lua_pushcfunction(GlobalLuaState, SetStringArray);
    lua_setglobal(GlobalLuaState, "SetStringArray");

    lua_pushcfunction(GlobalLuaState, Include);
    lua_setglobal(GlobalLuaState, "Include");


    if(luaL_dofile(GlobalLuaState, "../../game/settings.lua") != LUA_OK) 
    {
        luaL_error(GlobalLuaState, "Error reading SETTINGS file: %s\n", lua_tostring(GlobalLuaState, -1));
        return EXIT_FAILURE;
    }
    lua_getglobal(GlobalLuaState, "name");
    lua_getglobal(GlobalLuaState, "width");
    lua_getglobal(GlobalLuaState, "height");

    const char *WindowName = lua_tostring(GlobalLuaState, -3);
    WindowWidth = (i32)lua_tointeger(GlobalLuaState, -2);
    WindowHeight = (i32)lua_tointeger(GlobalLuaState, -1);
    
    Win32LoadXInput();
    HWND Window = Win32InitializeWindow(Instance, 0, 0, WindowWidth, WindowHeight, WindowName);
    HDC DeviceContext = GetDC(Window);
    Win32InitializeOpenGLContext(DeviceContext, WindowWidth, WindowHeight);
    stbi_set_flip_vertically_on_load(TRUE);
    ShowWindow(Window, true);
    
    LARGE_INTEGER Frequency = {};
    QueryPerformanceFrequency(&Frequency); 
    // TODO: Initialize
     
    // load assets
    if(luaL_dofile(GlobalLuaState, "../../game/assets.lua") != LUA_OK)
    {
        luaL_error(GlobalLuaState, "Error loading ASSETS file: %s\n", lua_tostring(GlobalLuaState, -1));
        return EXIT_FAILURE;
    }

    LoadTexturesToAtlas(GlobalTextureNames, GlobalTextureCount);
    texture *GrassTexture = TextureCreateFromBuffer((unsigned char *)GlobalTextureAtlas.Data,
                                                                     GlobalTextureAtlas.Width,
                                                                     GlobalTextureAtlas.Height, 4);

    if(luaL_dofile(GlobalLuaState, "../../game/main.lua") != LUA_OK) 
    {
        luaL_error(GlobalLuaState, "Error reading MAIN file: %s\n", lua_tostring(GlobalLuaState, -1));
        return EXIT_FAILURE;
    }
     
    input LastInput = {};
    input CurrInput = {};

    lua_getglobal(GlobalLuaState, "gRenderer");
    renderer *Renderer = (renderer *)lua_tointeger(GlobalLuaState, -1);

    shader *Shader = ShaderCreate("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    ShaderBind(Shader);
    UpdateInt(Shader, "texture1", 0);
    glm::mat4 ProjectionMat = glm::ortho(-(f32)WindowWidth*0.5f, (f32)WindowWidth*0.5f, -(f32)WindowHeight*0.5f, (f32)WindowHeight*0.5f, 0.0f, 100.0f);
    UpdateMat4f(Shader, "uProj", ProjectionMat);

    glm::vec3 Position = glm::vec3(0, 0, 20);
    glm::vec3 Front = glm::vec3(Position.x, Position.y, -1);
    glm::vec3 Up = glm::vec3(0, 1, 0);
    glm::mat4 ViewMat = glm::lookAt(Position, Front, Up);
    UpdateMat4f(Shader, "uView", ViewMat);

    LARGE_INTEGER LastCounter = {};
    QueryPerformanceCounter(&LastCounter);

    GlobalRunning = true;
    while(GlobalRunning)
    {
        LARGE_INTEGER CurrentCounter = {};
        QueryPerformanceCounter(&CurrentCounter);
        f64 Fps = (f64)Frequency.QuadPart / (f64)(CurrentCounter.QuadPart - LastCounter.QuadPart);
        f32 DeltaTime = (f32)((f64)(CurrentCounter.QuadPart - LastCounter.QuadPart) / (f64)Frequency.QuadPart);

        Win32ProcessMessages(&CurrInput, &LastInput);
 
        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Render
        glActiveTexture(GL_TEXTURE0);
        TextureBind(GrassTexture);

        RenderBegin(Renderer, Shader);
        lua_getglobal(GlobalLuaState, "update");
        if(lua_isfunction(GlobalLuaState, -1))
        {
            lua_pushnumber(GlobalLuaState, DeltaTime);
            lua_pcall(GlobalLuaState, 1, 0, 0);
        }

        RenderEnd(Renderer);

        TextureUnbind();

        SwapBuffers(DeviceContext); 
        LastInput = CurrInput;
        LastCounter = CurrentCounter;
    }
    
    RendererDestroy(Renderer);
    TextureDestroy(GrassTexture);
    ShaderDestroy(Shader);
    lua_close(GlobalLuaState);
    return 0;
}
