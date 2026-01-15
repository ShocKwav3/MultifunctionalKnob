#include "EncoderModeManager.h"
#include "Helper/EncoderModeHelper.h"
#include "Display/Model/DisplayRequest.h"
#include "state/HardwareState.h"

extern HardwareState hardwareState;

EncoderModeManager::EncoderModeManager(EncoderEventHandler* encoderEventHandler, EncoderModeSelector* encoderModeSelector)
    : encoderEventHandler(encoderEventHandler),
      encoderModeSelector(encoderModeSelector),
      currentMode(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL),
      previousMode(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL),
      displayQueue(nullptr) {}

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

    // Update global system state and refresh display
    hardwareState.encoderWheelState.mode = EncoderModeHelper::toWheelMode(mode);
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

void EncoderModeManager::setDisplayQueue(QueueHandle_t queue) {
    displayQueue = queue;
}

void EncoderModeManager::updateDisplayState() {
    if (displayQueue == nullptr) {
        return;  // Display queue not set yet
    }

    DisplayRequest request;
    request.type = DisplayRequestType::DRAW_NORMAL_MODE;
    request.data.normalMode.state = hardwareState;

    // Non-blocking send with short timeout
    xQueueSend(displayQueue, &request, pdMS_TO_TICKS(10));
}
