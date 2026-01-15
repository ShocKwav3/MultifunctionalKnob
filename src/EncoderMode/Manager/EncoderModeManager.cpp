#include "EncoderModeManager.h"
#include "Helper/EncoderModeHelper.h"
#include "Display/Model/DisplayRequest.h"
#include "state/HardwareState.h"

EncoderModeManager::EncoderModeManager(
    EncoderEventHandler* encoderEventHandler,
    EncoderModeSelector* encoderModeSelector,
    QueueHandle_t displayQueue,
    HardwareState* hwState
)
    : encoderEventHandler(encoderEventHandler),
      encoderModeSelector(encoderModeSelector),
      displayQueue(displayQueue),
      hardwareState(hwState),
      currentMode(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL),
      previousMode(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL) {}

void EncoderModeManager::registerHandler(EventEnum::EncoderModeEventTypes mode, EncoderModeHandlerInterface* handler) {
    if (static_cast<int>(mode) >= static_cast<int>(EventEnum::EncoderModeEventTypes::__ENCODER_MODE_SELECTION_LIMIT)) return;
    modeHandlers[static_cast<int>(mode)] = handler;
}

void EncoderModeManager::setCurrentHandler(EncoderModeBaseInterface* handler) {
    if (encoderEventHandler && handler) {
        encoderEventHandler->setModeHandler(handler);
    }
}

void EncoderModeManager::setMode(EventEnum::EncoderModeEventTypes mode) {
    if (static_cast<int>(mode) >= static_cast<int>(EventEnum::EncoderModeEventTypes::__ENCODER_MODE_SELECTION_LIMIT)) {
        Serial.println("EncoderModeManager: Invalid mode");
        return;
    }

    previousMode = currentMode;
    currentMode = mode;

    EncoderModeHandlerInterface* handler = modeHandlers[static_cast<int>(mode)];

    if (handler) {
        setCurrentHandler(handler);
    }

    // Update hardware state and refresh display
    hardwareState->encoderWheelState.mode = EncoderModeHelper::toWheelMode(mode);
    updateDisplayState();
}

void EncoderModeManager::enterModeSelection() {
    previousMode = currentMode;
    currentMode = EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION;

    if (encoderModeSelector) {
        setCurrentHandler(encoderModeSelector);
    }
}

void EncoderModeManager::cancelModeSelection() {
    setMode(previousMode);
}

void EncoderModeManager::updateDisplayState() {
    DisplayRequest request;
    request.type = DisplayRequestType::DRAW_NORMAL_MODE;
    request.data.normalMode.state = *hardwareState;

    // Non-blocking send with short timeout
    xQueueSend(displayQueue, &request, pdMS_TO_TICKS(10));
}
