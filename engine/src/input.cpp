global_variable input *GlobalInputPtr;

global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
global_variable u16 XInputButtons[] = 
{
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y
};

internal void Win32LoadXInput()
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
    }
    if(!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState"); 
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if(!XInputGetState) XInputGetState = XInputGetStateStub; 
        if(!XInputSetState) XInputSetState = XInputSetStateStub; 
    }
}

internal f32 Win32ProcessXInputStick(i16 Value, i32 DeadZoneValue)
{
    f32 Result = 0;
    if(Value < -DeadZoneValue)
    {
        Result = (f32)(Value + DeadZoneValue) / (32768.0f - DeadZoneValue);
    }
    else if(Value > DeadZoneValue)
    {
        Result = (f32)(Value - DeadZoneValue) / (32767.0f - DeadZoneValue);
    }
    return Result;
}


internal read_file_result Win32ReadEntireFile(const char *Filepath)
{ 
    read_file_result Result = {};
    HANDLE FileHandle = CreateFileA(Filepath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx(FileHandle, &FileSize))
        {
            Assert(FileSize.QuadPart <= 0xFFFFFFFF);
            Result.data = malloc(FileSize.QuadPart + 1);
            Result.size = FileSize.QuadPart;
            if(ReadFile(FileHandle, Result.data, (DWORD)Result.size, 0, 0))
            {
                u8 *LastByte = (u8 *)Result.data + Result.size;
                *LastByte = 0;
                return Result;
            }
        }
    }
    Result = {};
    return Result;
}

internal i32 GetKeyDown(lua_State *LuaState)
{
    unsigned int ASCII = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    lua_pushboolean(LuaState, Input->Keys[ASCII].IsDown);
    return 1;
}

internal i32 GetKeyJustDown(lua_State *LuaState)
{
    unsigned int ASCII = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    if(Input->Keys[ASCII].IsDown != Input->Keys[ASCII].WasDown)
    {
        lua_pushboolean(LuaState, Input->Keys[ASCII].IsDown);
    } 
    else
    { 
        lua_pushboolean(LuaState, false);
    }
    return 1;
}

internal i32 GetKeyUp(lua_State *LuaState)
{
    unsigned int ASCII = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    lua_pushboolean(LuaState, !Input->Keys[ASCII].IsDown);
    return 1;
}

internal i32 GetKeyJustUp(lua_State *LuaState)
{
    unsigned int ASCII = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    if(Input->Keys[ASCII].IsDown != Input->Keys[ASCII].WasDown)
    {
        lua_pushboolean(LuaState, Input->Keys[ASCII].WasDown);
    } 
    else
    { 
        lua_pushboolean(LuaState, false);
    }
    return 1;
}

internal i32 GetJoystickButtonDown(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    lua_pushboolean(LuaState, Input->Buttons[Button].IsDown);
    return 1;
}

internal i32 GetJoystickButtonJustDown(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    if(Input->Buttons[Button].IsDown != Input->Buttons[Button].WasDown)
    {
        lua_pushboolean(LuaState, Input->Buttons[Button].IsDown);
    } 
    else
    { 
        lua_pushboolean(LuaState, false);
    }
    return 1;
}

internal i32 GetJoystickButtonUp(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    lua_pushboolean(LuaState, !Input->Buttons[Button].IsDown);
    return 1;
}

internal i32 GetJoystickButtonJustUp(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    if(Input->Buttons[Button].IsDown != Input->Buttons[Button].WasDown)
    {
        lua_pushboolean(LuaState, Input->Buttons[Button].WasDown);
    } 
    else
    { 
        lua_pushboolean(LuaState, false);
    }
    return 1;
}

internal i32 GetMouseButtonDown(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    lua_pushboolean(LuaState, Input->MouseButtons[Button].IsDown);
    return 1;
}

internal i32 GetMouseButtonJustDown(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    if(Input->MouseButtons[Button].IsDown != Input->MouseButtons[Button].WasDown)
    {
        lua_pushboolean(LuaState, Input->MouseButtons[Button].IsDown);
    } 
    else
    { 
        lua_pushboolean(LuaState, false);
    }
    return 1;
}

internal i32 GetMouseButtonUp(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    lua_pushboolean(LuaState, !Input->MouseButtons[Button].IsDown);
    return 1;
}

internal i32 GetMouseButtonJustUp(lua_State *LuaState)
{
    unsigned int Button = (unsigned int)lua_tointeger(LuaState, -1);
    input *Input = GlobalInputPtr;
    if(Input->MouseButtons[Button].IsDown != Input->MouseButtons[Button].WasDown)
    {
        lua_pushboolean(LuaState, Input->MouseButtons[Button].WasDown);
    } 
    else
    { 
        lua_pushboolean(LuaState, false);
    }
    return 1;
}

internal i32 GetMouseInfo(lua_State *LuaState)
{
    input *Input = GlobalInputPtr;
    lua_pushinteger(LuaState, Input->MouseX);
    lua_pushinteger(LuaState, Input->MouseY);
    return 2;
}

internal i32 GetJoystickLeftStickInfo(lua_State *LuaState)
{
    input *Input = GlobalInputPtr;
    lua_pushnumber(LuaState, Input->LeftStickX);
    lua_pushnumber(LuaState, Input->LeftStickY);
    return 2;
}

internal i32 GetJoystickRightStickInfo(lua_State *LuaState)
{
    input *Input = GlobalInputPtr;
    lua_pushnumber(LuaState, Input->RightStickX);
    lua_pushnumber(LuaState, Input->RightStickY);
    return 2;
}
