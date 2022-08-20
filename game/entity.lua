Entity = {}
Entity.__index = Entity
function Entity:Create(def)
    local this = 
    {
        mTexture = def.texture,
        mHeight = def.height,
        mWidth = def.width,
        mTileX = def.tileX,
        mTileY = def.tileY,
        mStartFrame = def.startFrame,
        mFrame = def.startFrame,
    }
    this.mUVs = GenerateUVs(this.mTexture, this.mWidth, this.mHeight)
    this.mPixelX = 0
    this.mPixelY = 0 
    setmetatable(this, self)
    this:SetFrame(this.mFrame)



    return this
end

function Entity:SetFrame(frame)
    self.mFrame = frame
end

function Entity:SetPosition(pixelX, pixelY)
    self.mPixelX = pixelX
    self.mPixelY = pixelY
end

function Entity:GetPosition()
    return {
        x = self.mPixelX,
        y = self.mPixelY
    }
end

function Entity:Draw(renderer)
    DrawSprite(renderer,
               self.mPixelX, self.mPixelY,
               self.mWidth, self.mHeight,
               0.0,  1.0,
               self.mTexture,
               table.unpack(self.mUVs[self.mFrame]))
end
