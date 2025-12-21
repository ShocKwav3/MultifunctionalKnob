#pragma once

#include "Arduino.h"

#include "EncoderModeHandlerAbstract.h"
#include "Enum/EventEnum.h"
#include "Helper/EncoderModeHelper.h"

class EncoderModeHandlerZoom : public EncoderModeHandlerAbstract {
public:
    EncoderModeHandlerZoom(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    const char* getModeName() const override;
};
