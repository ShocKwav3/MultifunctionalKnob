#pragma once

#include "Arduino.h"

#include "Event/Dispatcher/AppEventDispatcher.h"
#include "Enum/EventEnum.h"
#include "Helper/EncoderModeHelper.h"
#include "EncoderMode/Interface/EncoderModeBaseInterface.h"

class EncoderModeSelector: public EncoderModeBaseInterface {
public:
    EncoderModeSelector(AppEventDispatcher* dispatcher);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    void handleLongClick()  override;

    const char* getModeName() const override;
private:
    int selectionIndex = 0;
    AppEventDispatcher* appEventDispatcher;
};
