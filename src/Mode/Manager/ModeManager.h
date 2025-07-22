#pragma once

#include "Arduino.h"

#include "Enum/EventEnum.h"
#include "Mode/Handler/ModeHandlerInterface.h"
#include "Mode/Handler/ModeSelectionHandler.h"
#include "Event/Handler/RotaryEncoderEventHandler.h"

class RotaryEncoderEventHandler;

class ModeManager {
public:
    ModeManager(RotaryEncoderEventHandler* rotaryHandler);

    void registerHandler(EventEnum::AppEventTypes mode, ModeHandlerInterface* handler);
    void setSelectionHandler(ModeSelectionHandler* handler);

    void setMode(EventEnum::AppEventTypes mode);
    void enterModeSelection();
    void cancelModeSelection();

private:
    EventEnum::AppEventTypes currentMode;
    EventEnum::AppEventTypes previousMode;

    ModeHandlerInterface* modeHandlers[static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT)] = {};
    ModeSelectionHandler* selectionHandler = nullptr;

    RotaryEncoderEventHandler* rotaryEventHandler;

    void setCurrentHandler(ModeHandlerInterface* handler);
};
