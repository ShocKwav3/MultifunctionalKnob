#include "EncoderModeManager.h"

EncoderModeManager::EncoderModeManager(EncoderEventHandler* encoderEventHandler)
    : encoderEventHandler(encoderEventHandler),
      currentMode(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL),
      previousMode(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL) {}

void EncoderModeManager::registerHandler(EventEnum::EncoderModeEventTypes mode, EncoderModeHandlerInterface* handler) {
    if (static_cast<int>(mode) >= static_cast<int>(EventEnum::EncoderModeEventTypes::__ENCODER_MODE_SELECTION_LIMIT)) return;
    modeHandlers[static_cast<int>(mode)] = handler;
}

void EncoderModeManager::setSelectionHandler(EncoderModeSelectionHandler* handler) {
    selectionHandler = handler;
}

void EncoderModeManager::setCurrentHandler(EncoderModeHandlerInterface* handler) {
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
}

void EncoderModeManager::enterModeSelection() {
    previousMode = currentMode;
    currentMode = EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION;

    if (selectionHandler) {
        setCurrentHandler(selectionHandler);
    }
}

void EncoderModeManager::cancelModeSelection() {
    setMode(previousMode);
}
