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
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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

void LoadEngineFunctionToLua()
{
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
    lua_pushcfunction(GlobalLuaState, RendererTranslate);
    lua_setglobal(GlobalLuaState, "RendererTranslate");
    lua_pushcfunction(GlobalLuaState, GetKeyDown);
    lua_setglobal(GlobalLuaState, "GetKeyDown");
    lua_pushcfunction(GlobalLuaState, GetKeyJustDown);
    lua_setglobal(GlobalLuaState, "GetKeyJustDown");
    lua_pushcfunction(GlobalLuaState, GetKeyUp);
    lua_setglobal(GlobalLuaState, "GetKeyUp");
    lua_pushcfunction(GlobalLuaState, GetKeyJustUp);
    lua_setglobal(GlobalLuaState, "GetKeyJustUp");
    lua_pushcfunction(GlobalLuaState, GetJoystickButtonDown);
    lua_setglobal(GlobalLuaState, "GetJoystickButtonDown");
    lua_pushcfunction(GlobalLuaState, GetJoystickButtonJustDown);
    lua_setglobal(GlobalLuaState, "GetJoystickButtonJustDown");
    lua_pushcfunction(GlobalLuaState, GetJoystickButtonUp);
    lua_setglobal(GlobalLuaState, "GetJoystickButtonUp");
    lua_pushcfunction(GlobalLuaState, GetJoystickButtonJustUp);
    lua_setglobal(GlobalLuaState, "GetJoystickButtonJustUp");
    lua_pushcfunction(GlobalLuaState, GetMouseButtonDown);
    lua_setglobal(GlobalLuaState, "GetMouseButtonDown");
    lua_pushcfunction(GlobalLuaState, GetMouseButtonJustDown);
    lua_setglobal(GlobalLuaState, "GetMouseButtonJustDown");
    lua_pushcfunction(GlobalLuaState, GetMouseButtonUp);
    lua_setglobal(GlobalLuaState, "GetMouseButtonUp");
    lua_pushcfunction(GlobalLuaState, GetMouseButtonJustUp);
    lua_setglobal(GlobalLuaState, "GetMouseButtonJustUp");
    lua_pushcfunction(GlobalLuaState, GetMouseInfo);
    lua_setglobal(GlobalLuaState, "GetMouseInfo");
    lua_pushcfunction(GlobalLuaState, GetJoystickLeftStickInfo);
    lua_setglobal(GlobalLuaState, "GetJoystickLeftStickInfo");
    lua_pushcfunction(GlobalLuaState, GetJoystickRightStickInfo);
    lua_setglobal(GlobalLuaState, "GetJoystickRightStickInfo");
}

/* THE ENTRY POINT OF THE ENGINE */
int main()
{
    HINSTANCE Instance = GetModuleHandle(0);

    // TODO: load C-API test...
    GlobalLuaState = luaL_newstate();
    luaL_openlibs(GlobalLuaState);
    LoadEngineFunctionToLua();

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

    input LastInput = {};
    input CurrInput = {};
    GlobalInputPtr = &CurrInput;

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
     
    lua_getglobal(GlobalLuaState, "gRenderer");
    renderer *Renderer = (renderer *)lua_tointeger(GlobalLuaState, -1);

    shader *Shader = ShaderCreate("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    ShaderBind(Shader);
    UpdateInt(Shader, "texture1", 0);
    glm::mat4 ProjectionMat = glm::ortho(-(f32)WindowWidth*0.5f, (f32)WindowWidth*0.5f, -(f32)WindowHeight*0.5f, (f32)WindowHeight*0.5f, 0.0f, 100.0f);
    UpdateMat4f(Shader, "uProj", ProjectionMat);

    LARGE_INTEGER LastCounter = {};
    QueryPerformanceCounter(&LastCounter);

    GlobalRunning = true;
    while(GlobalRunning)
    {
        glm::mat4 ViewMat = glm::lookAt(Renderer->Position,
                                        Renderer->Front,
                                        Renderer->Up);
        UpdateMat4f(Shader, "uView", ViewMat);

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

        Renderer->LastDrawCallCount = Renderer->CurrentDrawCallCount;
        Renderer->CurrentDrawCallCount = 0;

        //printf("DrawCallsPerFrame: %d\n", Renderer->LastDrawCallCount);
    }
    
    RendererDestroy(Renderer);
    TextureDestroy(GrassTexture);
    ShaderDestroy(Shader);
    lua_close(GlobalLuaState);
    return 0;
}
