#pragma once

#include "Arduino.h"

#include "Enum/EventEnum.h"
#include "EncoderMode/Handler/EncoderModeHandlerInterface.h"
#include "EncoderMode/Handler/EncoderModeSelectionHandler.h"
#include "Event/Handler/EncoderEventHandler.h"

class EncoderEventHandler;

class EncoderModeManager {
public:
    EncoderModeManager(EncoderEventHandler* encoderEventHandler);

    void registerHandler(EventEnum::AppEventTypes mode, EncoderModeHandlerInterface* handler);
    void setSelectionHandler(EncoderModeSelectionHandler* handler);

    void setMode(EventEnum::AppEventTypes mode);
    void enterModeSelection();
    void cancelModeSelection();

private:
    EventEnum::AppEventTypes currentMode;
    EventEnum::AppEventTypes previousMode;

    EncoderModeHandlerInterface* modeHandlers[static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT)] = {};
    EncoderModeSelectionHandler* selectionHandler = nullptr;

    EncoderEventHandler* encoderEventHandler;

    void setCurrentHandler(EncoderModeHandlerInterface* handler);
};
