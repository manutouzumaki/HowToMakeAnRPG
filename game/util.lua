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
