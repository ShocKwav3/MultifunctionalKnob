#include "ScrollModeHandler.h"

ScrollModeHandler::ScrollModeHandler(AppEventDispatcher* dispatcher)
    : AbstractModeHandler(dispatcher) {}

void ScrollModeHandler::handleRotate(int delta) {
    Serial.printf("ScrollModeHandler: Rotating by %d\n", delta);
    // TODO: Send BLE horizontal scroll event via bleKeyboard.mouseMove(...)
}

void ScrollModeHandler::handleShortClick() {
    Serial.println("ScrollModeHandler: Short click detected.");
    // TODO: Maybe toggle scroll direction or switch sub-mode
}

const char* ScrollModeHandler::getModeName() const {
    return ModeHelper::toString(EventEnum::AppEventTypes::SCROLL);
}
