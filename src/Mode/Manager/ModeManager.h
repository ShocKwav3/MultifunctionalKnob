#pragma once

#include "Arduino.h"

#include "Enum/ModeEnum.h"
#include "Mode/Handler/ModeHandlerInterface.h"
#include "Mode/Handler/ModeSelectionHandler.h"
#include "Event/Handler/RotaryEncoderEventHandler.h"

class RotaryEncoderEventHandler;

class ModeManager {
public:
    ModeManager(RotaryEncoderEventHandler* rotaryHandler);

    void registerHandler(ModeEnum::AppEventTypes mode, ModeHandlerInterface* handler);
    void setSelectionHandler(ModeSelectionHandler* handler);

    void setMode(ModeEnum::AppEventTypes mode);
    void enterModeSelection();
    void cancelModeSelection();

private:
    ModeEnum::AppEventTypes currentMode;
    ModeEnum::AppEventTypes previousMode;

    ModeHandlerInterface* modeHandlers[static_cast<int>(ModeEnum::AppEventTypes::__MODE_SELECTION_LIMIT)] = {};
    ModeSelectionHandler* selectionHandler = nullptr;

    RotaryEncoderEventHandler* rotaryEventHandler;

    void setCurrentHandler(ModeHandlerInterface* handler);
};
