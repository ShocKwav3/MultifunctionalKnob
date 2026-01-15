#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "Enum/EventEnum.h"
#include "EncoderMode/Handler/EncoderModeHandlerInterface.h"
#include "EncoderMode/Selector/EncoderModeSelector.h"
#include "EncoderMode/Interface/EncoderModeBaseInterface.h"
#include "Event/Handler/EncoderEventHandler.h"

// Forward declarations
class EncoderEventHandler;
struct HardwareState;

class EncoderModeManager {
public:
    EncoderModeManager(
        EncoderEventHandler* encoderEventHandler,
        EncoderModeSelector* encoderModeSelector,
        QueueHandle_t displayQueue,
        HardwareState* hwState
    );

    void registerHandler(EventEnum::EncoderModeEventTypes mode, EncoderModeHandlerInterface* handler);

    void setMode(EventEnum::EncoderModeEventTypes mode);
    void enterModeSelection();
    void cancelModeSelection();

private:
    EventEnum::EncoderModeEventTypes currentMode;
    EventEnum::EncoderModeEventTypes previousMode;

    EncoderModeHandlerInterface* modeHandlers[static_cast<int>(EventEnum::EncoderModeEventTypes::__ENCODER_MODE_SELECTION_LIMIT)] = {};
    EncoderModeSelector* encoderModeSelector = nullptr;

    EncoderEventHandler* encoderEventHandler;
    QueueHandle_t displayQueue;  ///< For sending state updates to display
    HardwareState* hardwareState;  ///< Hardware state for display requests

    void setCurrentHandler(EncoderModeBaseInterface* handler);
    void updateDisplayState();  // Helper to send DRAW_NORMAL_MODE request
};
