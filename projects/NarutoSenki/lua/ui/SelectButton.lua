--
-- SelectButton
--
SelectButton.Char_None = 'None'
SelectButton.Char_None2 = 'None2'

local unsupportedList = {
    -- Here are not supported or in dev characters
    'Pain', 'Orochimaru'
}

-- init values to lower case
for k, v in pairs(unsupportedList) do unsupportedList[k] = string.lower(v) end

function ns.addUnsupportCharacter(name)
    if name ~= nil then
        if table.has(name) then
            log('Add error %s already in unsupported table', name)
        else
            unsupportedList.insert(name)
        end
    end
end

SelectButton = class('SelectButton', function() return Button:create() end)

SelectButton.Type = {
    Menu = 0,
    Unlock1 = 1,
    Unlock2 = 2,
    skill1 = 3,
    skill2 = 4,
    skill3 = 5,
    Ougis1 = 6,
    Ougis2 = 7
}

function SelectButton:ctor()
    -- SelectButton.super.ctor(self)
    self:setPressedActionEnabled(false)

    local function touchEvent(sender, event)
        if event == ns.TouchEventType.began then
            -- button:setTitleText("Touch Down")
            self:click()
            log('SelectButton click [ ' .. self._charName .. ' ]')
        elseif event == ns.TouchEventType.moved then
            -- button:setTitleText("Touch Move")
        elseif event == ns.TouchEventType.ended then
            -- button:setTitleText("Touch Up")
        elseif event == ns.TouchEventType.canceled then
            -- button:setTitleText("Touch Cancelled")
        end
    end
    self:addTouchEventListener(touchEvent)

    self._btnType = SelectButton.Type.Menu
    self._isCanBuy = false
    self._isCanBuy2 = false
    self._isUnlock = false
    self._clickTime = 0
    self._isAviable = true
    self._selectLayer = nil
    self._skillLayer = nil
    self._charName = nil
end

function SelectButton:create(image)
    local button = SelectButton:new()
    button:loadTextures(image, image, nil, UI_TEX_TYPE_PLIST)
    button:setAnchorPoint(CCPoint(0, 0))
    return button
end

function SelectButton:click()
    if self._btnType ~= SelectButton.Type.Menu then
        self._skillLayer:setSkillExplain(self._btnType)
        return
    end

    if self._charName == SelectButton.Char_None2 then
        -- tools.setTip('LimitedChar')
        local tip = CCTips:create("LimitedChar")
        self._selectLayer:addChild(tip, 5000)
    elseif self._charName ~= SelectButton.Char_None and self._isAviable and
        self:isSupported(self._charName) then
        audio.stopAllSounds()

        if not self._selectLayer then
            -- audio.playSound(ns.menu.SELECT_SOUND)
            -- self._selectLayer:setSelected(self)
            log('SelectLayer can not be nil')
            return
        end

        if self._selectLayer._playerSelect and not ns.enableCustomSelect then
            return
        end

        if not self._selectLayer._com2Select then
            for _, selectBtn in pairs(self._selectLayer.selectButtons) do
                if selectBtn._charName == self._charName then
                    selectBtn._clickTime = 0
                end

                if self._clickTime == 0 then
                    if save.isVoice() then
                        audio.stopAllSounds()
                        audio.playSound(string.format('Audio/Intro/%s.ogg',
                                                      self._charName))
                    end
                    audio.playSound(ns.menu.SELECT_SOUND)
                    log('click %s 0 time', self._charName)
                elseif self._clickTime == 1 then
                    audio.playSound("Audio/Menu/confirm2.ogg")
                    self._isAviable = false
                    self:spriteToGrey()
                    self:getParent():reorderChild(self, 500)
                    log('click %s 0 time', self._charName)
                end

                self._clickTime = self._clickTime + 1
                self._selectLayer:setSelected(self)
                log('click %s 0 time', self._charName)
            end
        else
            log(not self._isAviable and 'Has selected character %s' or
                    'Not support %s ', self._charName)
        end
    end
end

function SelectButton:spriteToGrey()
    -- local black = display.newSprite('#Black_select.png')
    local black = ImageView:create()
    black:loadTexture('Black_select.png', UI_TEX_TYPE_PLIST)
    black:setAnchorPoint(CCPoint(0.5, 0.5))
    black:setPosition(CCPoint(17, 32))
    self:addChild(black)
end

function SelectButton:isSupported(val)
    for _, v in ipairs(unsupportedList) do
        if v == string.lower(val) then
            local tip = CCTips:create("LimitedChar")
            self._selectLayer:addChild(tip, 5000)
            return false
        end
    end
    return true
end
