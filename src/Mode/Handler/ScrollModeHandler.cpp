#include "ScrollModeHandler.h"

void ScrollModeHandler::handleRotate(int delta) {
    Serial.printf("ScrollModeHandler: Rotating by %d\n", delta);
    // TODO: Send BLE horizontal scroll event via bleKeyboard.mouseMove(...)
}

void ScrollModeHandler::handleShortClick() {
    Serial.println("ScrollModeHandler: Short click detected.");
    // TODO: Maybe toggle scroll direction or switch sub-mode
}
