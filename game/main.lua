Include("util.lua")
Include("statemachine.lua")
Include("tween.lua")
Include("animation.lua")
Include("map.lua")
Include("map_file.lua")
Include("entity.lua")
Include("movestate.lua")
Include("waitstate.lua")
Include("actions.lua")
Include("trigger.lua")

gRenderer = RendererCreate()
gDisplayWidth, gDisplayHeight = GetDisplayInfo()

local gMap = Map:Create(CreateMap6())
gMap:GotoTile(5, 5)


local heroDef =
{
    texture    = "walk_cycle.png",
    width      = 16,
    height     = 24,
    startFrame = 9,
    tileX      = 11,
    tileY      = 3,
    layer      = 1
}

gHero =
{
    mAnimUp = {1, 2, 3, 4},
    mAnimRight = {5, 6, 7, 8},
    mAnimDown = {9, 10, 11, 12},
    mAnimLeft = {13, 14, 15, 16},
    mFacing = "down",
    mEntity = Entity:Create(heroDef),
    Init =
    function(self)
        self.mController = StateMachine:Create
        {
            ['wait'] = function() return self.mWaitState end,
            ['move'] = function() return self.mMoveState end,
        }
        self.mWaitState = WaitState:Create(self, gMap)
        self.mMoveState = MoveState:Create(self, gMap)
        self.mController:Change("wait")
    end
}

gHero:Init()

function Teleport(entity, map)
    local x, y = map:GetTileFoot(entity.mTileX, entity.mTileY)
    entity:SetPosition(x, y + entity.mHeight / 2)
end

function GetFacedTileCoords(character)
    local xInc = 0
    local yInc = 0

    if character.mFacing == "left" then
        xInc = -1
    elseif character.mFacing == "right" then
        xInc = 1
    elseif character.mFacing == "up" then
        yInc = -1
    elseif character.mFacing == "down" then
        yInc = 1
    end

    local x = character.mEntity.mTileX + xInc
    local y = character.mEntity.mTileY + yInc

    return x, y
end


gUpDoorTeleport = Actions.Teleport(gMap, 11, 3)
gDownDoorTeleport = Actions.Teleport(gMap, 10, 11)
gUpDoorTeleport(nil, gHero.mEntity)


gTriggerTop = Trigger:Create
{
    OnEnter = gDownDoorTeleport
}

gTriggerBot = Trigger:Create
{
    OnEnter = gUpDoorTeleport
}

gTriggerStart = Trigger:Create
{
    OnExit = function() print("OnExit: Left the start position") end
}

gTriggerPot = Trigger:Create
{
    OnUse = function() print("OnUse: The pot is full of snakes!") end
}


gMap.mTriggers = 
{
    -- Layer 1
    {
        [gMap:CoordToIndex(10, 12)] = gTriggerBot,
        [gMap:CoordToIndex(11, 2)] = gTriggerTop,
        [gMap:CoordToIndex(11, 3)] = gTriggerStart,
        [gMap:CoordToIndex(10, 3)] = gTriggerPot,
    }
}

function update(dt)

    local playerPos = gHero.mEntity:GetPosition()
    gMap.mCamX = math.floor(playerPos.x)
    gMap.mCamY = math.floor(playerPos.y)
    RendererTranslate(gRenderer, gMap.mCamX, gMap.mCamY)
   
    if GetKeyJustDown(KEYBOARD_KEY_SPACE) or GetJoystickButtonJustDown(JOYSTICK_BUTTON_A) then
        local x, y = GetFacedTileCoords(gHero)
        local trigger = gMap:GetTrigger(gHero.mEntity.mLayer, x, y)
        if trigger then
            trigger:OnUse(gHero)
        end
    end

    local layerCount = gMap:LayerCount()
    for i = 1, layerCount do
        gMap:RenderLayer(gRenderer, i)
        if i == gHero.mEntity.mLayer then
            gHero.mEntity:Draw(gRenderer)
        end
    end

    gHero.mController:Update(dt)

end
