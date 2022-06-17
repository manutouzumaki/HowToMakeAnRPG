#ifndef INPUT_H
#define INPUT_H

struct button_state
{
    b8 IsDown;
    b8 WasDown;
};

struct input
{
    button_state Keys[350];

    i32 MouseX;
    i32 MouseY;

    f32 LeftStickX;
    f32 LeftStickY;
    f32 RightStickX;
    f32 RightStickY;
    union
    {
        struct
        {
            button_state Up;   
            button_state Down;
            button_state Left;
            button_state Right;
            button_state Start;
            button_state Back;
            button_state A;
            button_state B;
            button_state X;
            button_state Y;
        }; 
        button_state Buttons[10];
    }; 
    union
    { 
        struct
        {
            button_state MouseLeft;
            button_state MouseMiddle;
            button_state MouseRight;
        };
        button_state MouseButtons[3];
    };
};

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

#endif
