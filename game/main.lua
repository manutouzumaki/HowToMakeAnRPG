Include("test.lua")

xPos = 0
yPos = 0
COLOR0 = {r=0.6, g=0.2, b=0.2}
COLOR1 = {r=0.3, g=0.2, b=0.5}

gRenderer = RendererCreate()

gWidth, gHeight = GetTextureInfo("windows.png")
gWindowWidth, gWindowHeight = GetDisplayInfo();
print(gWindowWidth)
print(gWindowHeight)
print(pi)

function GenerateUVs(textureWidth, textureHeight, tileSize)
    local uvs = {}

    local width = tileSize / textureWidth;
    local height = tileSize / textureHeight;
    local cols = textureWidth / tileSize;
    local rows = textureHeight / tileSize;

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

gWindowsUVs = GenerateUVs(gWidth, gHeight, 32)

function update(dt)
    local speed = 10
    DrawBox2d(gRenderer, xPos, 0, 100, 50, COLOR0)
    DrawBox2d(gRenderer, 0, yPos, 100, 50, COLOR1)
    xPos = xPos + speed * dt
    yPos = yPos - speed * dt

    DrawTexture(gRenderer, -100 + xPos, 0, 100, 100, "grass_tile.png");
    
    DrawSprite(gRenderer, -100, 100, 100, 100, "windows.png", table.unpack(gWindowsUVs[4]));

end
