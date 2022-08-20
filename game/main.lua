Include("util.lua")
Include("statemachine.lua")
Include("tween.lua")
Include("animation.lua")
Include("map.lua")
Include("map_file.lua")
Include("entity.lua")
Include("movestate.lua")
Include("waitstate.lua")

gRenderer = RendererCreate()
gDisplayWidth, gDisplayHeight = GetDisplayInfo()

local gMap = Map:Create(CreateMap3())
gMap:GotoTile(5, 5)


local heroDef =
{
    texture    = "walk_cycle.png",
    width      = 16,
    height     = 24,
    startFrame = 9,
    tileX      = 10,
    tileY      = 2
}

gHero =
{
    mAnimUp = {1, 2, 3, 4},
    mAnimRight = {5, 6, 7, 8},
    mAnimDown = {9, 10, 11, 12},
    mAnimLeft = {13, 14, 15, 16},
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

Teleport(gHero.mEntity, gMap)

function update(dt)
    local playerPos = gHero.mEntity:GetPosition()
    gMap.mCamX = math.floor(playerPos.x)
    gMap.mCamY = math.floor(playerPos.y)
    RendererTranslate(gRenderer, gMap.mCamX, gMap.mCamY)

    gMap:Render(gRenderer) 
    gHero.mEntity:Draw(gRenderer)

    gHero.mController:Update(dt)

end
