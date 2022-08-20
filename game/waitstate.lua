WaitState = { mName = "wait" }
WaitState.__index = WaitState
function WaitState:Create(character, map)
    local this =
    {
        mCharacter = character,
        mMap = map,
        mEntity = character.mEntity,
        mController = character.mController,
        mFrameResetSpeed = 0.05,
        mFrameCount = 0,
    }
    setmetatable(this, self)
    return this
end

function WaitState:Enter(data)
    self.mFrameCount = 0
end

function WaitState:Render(renderer) end
function WaitState:Exit() end

function WaitState:Update(dt)

    if self.mFrameCount ~= -1 then
        self.mFrameCount = self.mFrameCount + dt
        if self.mFrameCount >= self.mFrameResetSpeed then
            self.mFrameCount = -1
            self.mEntity:SetFrame(self.mEntity.mStartFrame)
        end
    end

    local leftStickX, leftStickY = GetJoystickLeftStickInfo()
    if GetKeyDown('A') or GetJoystickButtonDown(JOYSTICK_BUTTON_LEFT) then
        self.mController:Change("move", {x = -1, y = 0})
    elseif GetKeyDown('D') or GetJoystickButtonDown(JOYSTICK_BUTTON_RIGHT) then
        self.mController:Change("move", {x = 1, y = 0})
    elseif GetKeyDown('W') or GetJoystickButtonDown(JOYSTICK_BUTTON_UP) then
        self.mController:Change("move", {x = 0, y = -1})
    elseif GetKeyDown('S') or GetJoystickButtonDown(JOYSTICK_BUTTON_DOWN) then
        self.mController:Change("move", {x = 0, y = 1})
    elseif leftStickX < -0.6 then
        self.mController:Change("move", {x = -1, y = 0})
    elseif leftStickX > 0.6 then
        self.mController:Change("move", {x = 1, y = 0})
    elseif leftStickY > 0.6 then
        self.mController:Change("move", {x = 0, y = -1})
    elseif leftStickY < -0.6 then
        self.mController:Change("move", {x = 0, y = 1})
    end
end
