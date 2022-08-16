--TODO:
--include("renderer.lua")

xPos = 0
yPos = 0
COLOR0 = {r=0.6, g=0.2, b=0.2}
COLOR1 = {r=0.3, g=0.2, b=0.5}

gRenderer = CreateRenderer()

gPoints = {0.1, 0.2, 0.3, 0.4, 1.5, 6.7, 0.27}
SetFloatArray(gPoints)

function update(dt)
    local speed = 10
    DrawBox2d(gRenderer, xPos, 0, 100, 50, COLOR0)
    DrawBox2d(gRenderer, 0, yPos, 100, 50, COLOR1)
    xPos = xPos + speed * dt
    yPos = yPos - speed * dt

    print(1.0/dt)
end
