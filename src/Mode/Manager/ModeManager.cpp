#include "ModeManager.h"

ModeManager::ModeManager(RotaryEncoderEventHandler* rotaryHandler)
    : rotaryEventHandler(rotaryHandler),
      currentMode(EventEnum::AppEventTypes::SCROLL),
      previousMode(EventEnum::AppEventTypes::SCROLL) {}

void ModeManager::registerHandler(EventEnum::AppEventTypes mode, ModeHandlerInterface* handler) {
    if (static_cast<int>(mode) >= static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT)) return;
    modeHandlers[static_cast<int>(mode)] = handler;
}

void ModeManager::setSelectionHandler(ModeSelectionHandler* handler) {
    selectionHandler = handler;
}

void ModeManager::setCurrentHandler(ModeHandlerInterface* handler) {
    if (rotaryEventHandler && handler) {
        rotaryEventHandler->setModeHandler(handler);
    }
}

void ModeManager::setMode(EventEnum::AppEventTypes mode) {
    if (static_cast<int>(mode) >= static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT)) {
        Serial.println("ModeManager: Invalid mode");
        return;
    }

    previousMode = currentMode;
    currentMode = mode;

    ModeHandlerInterface* handler = modeHandlers[static_cast<int>(mode)];

    if (handler) {
        setCurrentHandler(handler);
    }
}

void ModeManager::enterModeSelection() {
    previousMode = currentMode;
    currentMode = EventEnum::AppEventTypes::MODE_SELECTION;

    if (selectionHandler) {
        setCurrentHandler(selectionHandler);
    }
}

void ModeManager::cancelModeSelection() {
    setMode(previousMode);
}
