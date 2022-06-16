#include <Windows.h>
#include <Windowsx.h>
#include "defines.h"

#include <glad/glad.h>
#include <GL/wglext.h>

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

i32 WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, i32 CmdShow)
{
    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor( 0, IDC_ARROW );
    WindowClass.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    WindowClass.lpszMenuName = 0;
    WindowClass.lpszClassName = "HowToMakeAnRPG";

    RegisterClassEx(&WindowClass);

    RECT Rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect( &Rect, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, false );

    HWND Window = CreateWindowA("HowToMakeAnRPG", "HowToMakeAnRPG",
                            WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            Rect.right - Rect.left,
                            Rect.bottom - Rect.top,
                            0, 0, Instance, 0);

    ShowWindow(Window, CmdShow);    
    HDC DeviceContext = GetDC(Window);

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
            return 1;
        }    
    }
 
    GlobalRunning = true;
    while(GlobalRunning)
    {
        MSG Msg = {};
        while(PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }

        glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        SwapBuffers(DeviceContext);

    }
    return 0;
}
