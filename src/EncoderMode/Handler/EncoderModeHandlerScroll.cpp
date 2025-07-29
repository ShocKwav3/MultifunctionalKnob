#include "EncoderModeHandlerScroll.h"

EncoderModeHandlerScroll::EncoderModeHandlerScroll(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard)
    : EncoderModeHandlerAbstract(dispatcher, bleKeyboard){}

void EncoderModeHandlerScroll::handleRotate(int delta) {
    Serial.printf("EncoderModeHandlerScroll: Rotating by %d\n", delta);

    if (isVerticalScroll) {
        bleKeyboard->mouseMove(0, 0, delta, 0);

        return;
    }

    bleKeyboard->mouseMove(0, 0, 0, delta);
}

void EncoderModeHandlerScroll::handleShortClick() {
    Serial.println("EncoderModeHandlerScroll: Short click detected.");

    isVerticalScroll = !isVerticalScroll;
}

const char* EncoderModeHandlerScroll::getModeName() const {
    return EncoderModeHelper::toString(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL);
}
