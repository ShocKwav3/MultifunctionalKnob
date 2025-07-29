#pragma once

#include "Arduino.h"

#include "EncoderModeHandlerAbstract.h"
#include "Enum/EventEnum.h"
#include "Helper/EncoderModeHelper.h"

class EncoderModeHandlerVolume : public EncoderModeHandlerAbstract {
public:
    EncoderModeHandlerVolume(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    const char* getModeName() const override;
};
