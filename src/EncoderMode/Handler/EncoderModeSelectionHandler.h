#pragma once

#include "Arduino.h"

#include "EncoderModeHandlerAbstract.h"
#include "Enum/EventEnum.h"
#include "Helper/EncoderModeHelper.h"

class EncoderModeSelectionHandler : public EncoderModeHandlerAbstract {
public:
    EncoderModeSelectionHandler(AppEventDispatcher* dispatcher);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    void handleLongClick() override;
    const char* getModeName() const override;
private:
    int selectionIndex = 0;
};
