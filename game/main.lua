Include("util.lua")
Include("map.lua")
Include("map_file.lua")

gRenderer = RendererCreate()
gDisplayWidth, gDisplayHeight = GetDisplayInfo()

local gMap = Map:Create(CreateMap2())

--gMap:GotoTile(3, 6)

function update(dt)
    RendererTranslate(gRenderer, gMap.mCamX, gMap.mCamY)
    gMap:Render(gRenderer)

    if GetKeyDown('A') then
        gMap.mCamX = gMap.mCamX - 1
    elseif GetKeyDown('D') then
        gMap.mCamX = gMap.mCamX + 1
    end

    if GetKeyDown('W') then
        gMap.mCamY = gMap.mCamY + 1
    elseif GetKeyDown('S') then
        gMap.mCamY = gMap.mCamY - 1
    end

    leftStickX, leftStickY = GetJoystickLeftStickInfo()
    
    gMap.mCamX = gMap.mCamX + leftStickX
    gMap.mCamY = gMap.mCamY + leftStickY

end
