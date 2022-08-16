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

