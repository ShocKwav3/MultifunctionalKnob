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

    void registerHandler(EventEnum::EncoderModeEventTypes mode, EncoderModeHandlerInterface* handler);
    void setSelectionHandler(EncoderModeSelectionHandler* handler);

    void setMode(EventEnum::EncoderModeEventTypes mode);
    void enterModeSelection();
    void cancelModeSelection();

private:
    EventEnum::EncoderModeEventTypes currentMode;
    EventEnum::EncoderModeEventTypes previousMode;

    EncoderModeHandlerInterface* modeHandlers[static_cast<int>(EventEnum::EncoderModeEventTypes::__ENCODER_MODE_SELECTION_LIMIT)] = {};
    EncoderModeSelectionHandler* selectionHandler = nullptr;

    EncoderEventHandler* encoderEventHandler;

    void setCurrentHandler(EncoderModeHandlerInterface* handler);
};
