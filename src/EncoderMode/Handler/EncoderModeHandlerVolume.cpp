#include "EncoderModeHandlerVolume.h"

EncoderModeHandlerVolume::EncoderModeHandlerVolume(AppEventDispatcher* dispatcher)
    : EncoderModeHandlerAbstract(dispatcher) {}

void EncoderModeHandlerVolume::handleRotate(int delta) {
    Serial.printf("EncoderModeHandlerVolume: Rotating by %d\n", delta);
    // TODO: Send BLE horizontal scroll event via bleKeyboard.mouseMove(...)
}

void EncoderModeHandlerVolume::handleShortClick() {
    Serial.println("EncoderModeHandlerVolume: Short click detected.");
    // TODO: Maybe toggle scroll direction or switch sub-mode
}

const char* EncoderModeHandlerVolume::getModeName() const {
    return EncoderModeHelper::toString(EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME);
}
