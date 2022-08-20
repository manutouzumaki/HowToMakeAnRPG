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

        mTiles = layer.data,
        mTileWidth = mapDef.tilewidth,
        mTileHeight = mapDef.tileheight
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

function Map:GetTile(x, y)
    x = x + 1
    return self.mTiles[x + y * self.mWidth]
end

function Map:Render(renderer)
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
            local tile = self:GetTile(i, j)
            local uvs = self.mUVs[tile] 
            DrawSprite(renderer,
                       self.mX + i * self.mTileWidth,
                       self.mY - j * self.mTileHeight,
                       self.mTileWidth, self.mTileHeight,
                       self.mTextureAtlas,
                       table.unpack(uvs))

        end
    end
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
