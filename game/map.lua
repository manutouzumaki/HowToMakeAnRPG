Map = {}

Map.__index = Map
function Map:Create(mapDef)
    local layer = mapDef.layers[1]
    local this = {
        mX = 0,
        mY = 0,

        mMapDef = mapDef,
        mTextureAtlas = mapDef.tilesets[1].image,

        mLayer = layer,
        mWidth = layer.width,
        mHeight = layer.height,        
        mTileWidth = mapDef.tilewidth,
        mTileHeight = mapDef.tileheight,
        mTriggers = {},
    }
    local displayWidth, displayHeight = GetDisplayInfo()
    this.mX = -displayWidth / 2 + this.mTileWidth / 2
    this.mY = displayHeight / 2 - this.mTileHeight / 2

    -- To track the camera position
    this.mCamX = 0
    this.mCamY = 0

    -- Additional fields
    this.mWidthPixel = this.mWidth * this.mTileWidth
    this.mHeightPixel = this.mHeight * this.mTileHeight
    this.mUVs = GenerateUVs(this.mTextureAtlas, this.mTileWidth, this.mTileHeight)

    -- Assign blocking tile id
    for _, v in ipairs(mapDef.tilesets) do
        if v.name == "collision_graphic" then
            this.mBlockingTile = v.firstgid
        end
    end
    assert(this.mBlockingTile)

    setmetatable(this, self)
    return this
end

function Map:PointToTile(x, y)
    -- Tiles are rendered from the center so we adjust for this
    
    x = x + (self.mTileWidth / 2)
    y = y - (self.mTileHeight / 2)
    
    -- Clamp the point to the bounds of the map
    x = math.max(self.mX, x)
    y = math.min(self.mY, y)
    x = math.min(self.mX + self.mWidthPixel - 1, x)
    y = math.max(self.mY - self.mHeightPixel + 1, y)
    -- Map from bounded point to a tile
    local tileX = math.floor((x - self.mX) / self.mTileWidth)
    local tileY = math.floor((self.mY - y) / self.mTileHeight)

    return tileX, tileY
end

function Map:GetTile(x, y, layer)
    local layer = layer or 1
    local tiles = self.mMapDef.layers[layer].data
    return tiles[self:CoordToIndex(x, y)]
end

function Map:CoordToIndex(x, y)
    x = x + 1
    return x + y * self.mWidth
end

function Map:IsBlocked(layer, tileX, tileY)
    -- Collision layer should always be 1 above the official layer
    local tile = self:GetTile(tileX, tileY, layer + 2)
    return tile == self.mBlockingTile
end

function Map:Render(rendered)
    self:RenderLayer(rendered, 1)
end

function Map:RenderLayer(renderer, layer)
    
    local layerIndex = (layer * 3) - 2
    -- Get the topleft and bottomright pixel of the camera
    -- and use to get the tile

    local tileLeft, tileBottom =
        self:PointToTile(self.mCamX - (gDisplayWidth / 2),
                         self.mCamY - (gDisplayHeight / 2))
    local tileRight, tileTop =
        self:PointToTile(self.mCamX + (gDisplayWidth / 2),
                         self.mCamY + (gDisplayHeight / 2))

    for j = tileTop, tileBottom do
        for i = tileLeft, tileRight do

            local xPos = self.mX + i * self.mTileWidth
            local yPos = self.mY - j * self.mTileHeight
            local uvs = {}
            local tile = self:GetTile(i, j, layerIndex)

            if tile > 0 then
                uvs = self.mUVs[tile]
                DrawSprite(renderer,
                           xPos, yPos,
                           self.mTileWidth, self.mTileHeight,
                           0.0, 1.0,
                           self.mTextureAtlas,
                           table.unpack(uvs))
            end
        
            tile = self:GetTile(i, j, layerIndex + 1)
            if tile > 0 then
                uvs = self.mUVs[tile]
                DrawSprite(renderer,
                           xPos, yPos,
                           self.mTileWidth, self.mTileHeight,
                           0.0, 1.0,
                           self.mTextureAtlas,
                           table.unpack(uvs))
            end

        end
    end
end

function Map:LayerCount()
    assert(#self.mMapDef.layers % 3 == 0)
    return #self.mMapDef.layers / 3
end

function Map:Goto(x, y)
    self.mCamX = x - gDisplayWidth / 2
    self.mCamY = -y + gDisplayHeight / 2
end

function Map:GotoTile(x, y)
    self:Goto((x * self.mTileWidth) + self.mTileWidth / 2,
              (y * self.mTileHeight) + self.mTileHeight / 2)
end

function Map:GetTileFoot(x, y)
    return self.mX + (x *  self.mTileWidth), self.mY - (y * self.mTileHeight) - self.mTileHeight / 2
end

function Map:GetTrigger(layer, x, y)

    local triggers = self.mTriggers[layer]
    
    if not triggers then
        return
    end
    
    local index = self:CoordToIndex(x, y)
    return triggers[index]

end
