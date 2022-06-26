#include "includes.h"

global_variable bool GlobalRunning;

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

internal void Win32InitializeOpenGLContext(HDC DeviceContext)
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
            wglSwapIntervalEXT(0);
        }
    
        if(!gladLoadGL())
        {
            OutputDebugString("Error: glad failed Initialize\n");
        }
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
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

int main()
{
    HINSTANCE Instance = GetModuleHandle(0);

    Win32LoadXInput();
    HWND Window = Win32InitializeWindow(Instance, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "HowToMakeAnRPG");
    HDC DeviceContext = GetDC(Window);
    Win32InitializeOpenGLContext(DeviceContext);
    ShowWindow(Window, true);
    
    LARGE_INTEGER Frequency = {};
    QueryPerformanceFrequency(&Frequency);
    
    
    // TODO: Initialize 
    input LastInput = {};
    input CurrInput = {};

    u32 Shader = CreateShader("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    renderer Renderer = CreateRenderer();

    BindShader(Shader);
    glm::mat4 ProjectionMat = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, 0.0f, 100.0f);
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
 
        glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Render
        RenderBegin(&Renderer, Shader);
        for(i32 X = 0; X < 100; ++X)
        {
            for(i32 Y = 0; Y < 100; ++Y)
            {
                float ColorR = (float)X / 100.0f;
                float ColorG = (float)Y / 100.0f;
                AddQuadToRenderQueue(&Renderer, (float)X*8.0f, (float)Y*8.0f, 8, 8, 0, ColorR, ColorG, 0, 1);
            }
        }
        
        AddQuadToRenderQueue(&Renderer, 200.0f, 200.0f, 100, 100, 0, 0, 1, 0, 1);
        RenderEnd(&Renderer);

  
        SwapBuffers(DeviceContext); 
        LastInput = CurrInput;
        LastCounter = CurrentCounter;
        

        printf("fps:%d\n", (int)Fps);
    }
    ShutdownRenderer(&Renderer);
    return 0;
}
