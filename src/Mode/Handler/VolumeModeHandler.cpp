#include "VolumeModeHandler.h"

VolumeModeHandler::VolumeModeHandler(AppEventDispatcher* dispatcher)
    : AbstractModeHandler(dispatcher) {}

void VolumeModeHandler::handleRotate(int delta) {
    Serial.printf("VolumeModeHandler: Rotating by %d\n", delta);
    // TODO: Send BLE horizontal scroll event via bleKeyboard.mouseMove(...)
}

void VolumeModeHandler::handleShortClick() {
    Serial.println("VolumeModeHandler: Short click detected.");
    // TODO: Maybe toggle scroll direction or switch sub-mode
}

const char* VolumeModeHandler::getModeName() const {
    return ModeHelper::toString(EventEnum::AppEventTypes::VOLUME);
}
