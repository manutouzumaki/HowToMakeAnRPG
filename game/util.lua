JOYSTICK_BUTTON_UP    = 0
JOYSTICK_BUTTON_DOWN  = 1
JOYSTICK_BUTTON_LEFT  = 2
JOYSTICK_BUTTON_RIGHT = 3
JOYSTICK_BUTTON_START = 4
JOYSTICK_BUTTON_BACK  = 5
JOYSTICK_BUTTON_A     = 6
JOYSTICK_BUTTON_B     = 7
JOYSTICK_BUTTON_X     = 8
JOYSTICK_BUTTON_Y     = 9

KEYBOARD_KEY_BACKESPACE	= 0x08
KEYBOARD_KEY_ESCAPE     = 0x1B
KEYBOARD_KEY_0 = 0x30 
KEYBOARD_KEY_1 = 0x31 
KEYBOARD_KEY_2 = 0x32 
KEYBOARD_KEY_3 = 0x33 
KEYBOARD_KEY_4 = 0x34 
KEYBOARD_KEY_5 = 0x35 
KEYBOARD_KEY_6 = 0x36 
KEYBOARD_KEY_7 = 0x37 
KEYBOARD_KEY_8 = 0x38 
KEYBOARD_KEY_9 = 0x39 
KEYBOARD_KEY_A = 0x41 
KEYBOARD_KEY_B = 0x42 
KEYBOARD_KEY_C = 0x43 
KEYBOARD_KEY_D = 0x44 
KEYBOARD_KEY_E = 0x45 
KEYBOARD_KEY_F = 0x46 
KEYBOARD_KEY_G = 0x47 
KEYBOARD_KEY_H = 0x48 
KEYBOARD_KEY_I = 0x49 
KEYBOARD_KEY_J = 0x4A 
KEYBOARD_KEY_K = 0x4B 
KEYBOARD_KEY_L = 0x4C 
KEYBOARD_KEY_M = 0x4D 
KEYBOARD_KEY_N = 0x4E 
KEYBOARD_KEY_O = 0x4F 
KEYBOARD_KEY_P = 0x50 
KEYBOARD_KEY_Q = 0x51 
KEYBOARD_KEY_R = 0x52 
KEYBOARD_KEY_S = 0x53 
KEYBOARD_KEY_T = 0x54 
KEYBOARD_KEY_U = 0x55 
KEYBOARD_KEY_V = 0x56 
KEYBOARD_KEY_W = 0x57 
KEYBOARD_KEY_X = 0x58 
KEYBOARD_KEY_Y = 0x59 
KEYBOARD_KEY_Z = 0x5A 
KEYBOARD_KEY_NUMPAD0 = 0x60	    
KEYBOARD_KEY_NUMPAD1 = 0x61	    
KEYBOARD_KEY_NUMPAD2 = 0x62	    
KEYBOARD_KEY_NUMPAD3 = 0x63	    
KEYBOARD_KEY_NUMPAD4 = 0x64	    
KEYBOARD_KEY_NUMPAD5 = 0x65	    
KEYBOARD_KEY_NUMPAD6 = 0x66	    
KEYBOARD_KEY_NUMPAD7 = 0x67	    
KEYBOARD_KEY_NUMPAD8 = 0x68	    
KEYBOARD_KEY_NUMPAD9 = 0x69	    
KEYBOARD_KEY_RETURN  = 0x0D
KEYBOARD_KEY_SPACE   = 0x20
KEYBOARD_KEY_TAB     = 0x09
KEYBOARD_KEY_CONTROL = 0x11
KEYBOARD_KEY_SHIFT   = 0x10
KEYBOARD_KEY_ALT     = 0x12
KEYBOARD_KEY_CAPS    = 0x14
KEYBOARD_KEY_LEFT    = 0x25
KEYBOARD_KEY_UP      = 0x26
KEYBOARD_KEY_RIGHT   = 0x27
KEYBOARD_KEY_DOWN    = 0x28

MOUSE_BUTTON_LEFT   = 0
MOUSE_BUTTON_MIDDLE = 1
MOUSE_BUTTON_RIGHT  = 2

MATH_PI  = 3.14159265359
MATH_TAU = 6.28318530718

function GenerateUVs(texture, tileWidth, tileHeight)
    local uvs = {}
    local textureWidth, textureHeight = GetTextureInfo(texture)
    local width = tileWidth / textureWidth;
    local height = tileHeight / textureHeight;
    local cols = textureWidth / tileWidth;
    local rows = textureHeight / tileHeight;

    local u0 = 0
    local v0 = 0
    local u1 = width;
    local v1 = height;

    for j = 0, rows - 1 do
        for i = 0, cols - 1 do
            table.insert(uvs, {u0, v0, u1, v1})
            u0 = u0 + width
            u1 = u1 + width
        end
        u0 = 0
        v0 = v0 + height
        u1 = width
        v1 = v1 + height
    end
    return uvs
end

function DrawText(renderer, x, y, w, h, texture, uvs, text)
    for i = 1, #text do
        local letter = string.sub(text, i, i)
        local ascii = string.byte(letter) - 31;
        local xOffset = (i - 1) * w;
        DrawSprite(renderer, x + xOffset, y, w, h, texture, table.unpack(uvs[ascii]));
    end
end
