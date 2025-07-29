#include "EncoderModeHandlerScroll.h"

EncoderModeHandlerScroll::EncoderModeHandlerScroll(AppEventDispatcher* dispatcher)
    : EncoderModeHandlerAbstract(dispatcher) {}

void EncoderModeHandlerScroll::handleRotate(int delta) {
    Serial.printf("EncoderModeHandlerScroll: Rotating by %d\n", delta);
    // TODO: Send BLE horizontal scroll event via bleKeyboard.mouseMove(...)
}

void EncoderModeHandlerScroll::handleShortClick() {
    Serial.println("EncoderModeHandlerScroll: Short click detected.");
    // TODO: Maybe toggle scroll direction or switch sub-mode
}

const char* EncoderModeHandlerScroll::getModeName() const {
    return EnumToStringHelper::toString(EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SCROLL);
}
