#pragma once

#include "Arduino.h"

#include "EncoderModeHandlerAbstract.h"
#include "Enum/EventEnum.h"
#include "Helper/EnumToStringHelper.h"

class EncoderModeHandlerScroll : public EncoderModeHandlerAbstract {
public:
    EncoderModeHandlerScroll(AppEventDispatcher* dispatcher);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    const char* getModeName() const override;
};
