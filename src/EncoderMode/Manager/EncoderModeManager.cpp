#include "EncoderModeManager.h"

EncoderModeManager::EncoderModeManager(EncoderEventHandler* encoderEventHandler)
    : encoderEventHandler(encoderEventHandler),
      currentMode(EventEnum::AppEventTypes::SCROLL),
      previousMode(EventEnum::AppEventTypes::SCROLL) {}

void EncoderModeManager::registerHandler(EventEnum::AppEventTypes mode, EncoderModeHandlerInterface* handler) {
    if (static_cast<int>(mode) >= static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT)) return;
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

void EncoderModeManager::setMode(EventEnum::AppEventTypes mode) {
    if (static_cast<int>(mode) >= static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT)) {
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
    currentMode = EventEnum::AppEventTypes::MODE_SELECTION;

    if (selectionHandler) {
        setCurrentHandler(selectionHandler);
    }
}

void EncoderModeManager::cancelModeSelection() {
    setMode(previousMode);
}
