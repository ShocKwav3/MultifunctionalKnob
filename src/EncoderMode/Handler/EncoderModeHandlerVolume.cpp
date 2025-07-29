#include "EncoderModeHandlerVolume.h"

EncoderModeHandlerVolume::EncoderModeHandlerVolume(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard)
    : EncoderModeHandlerAbstract(dispatcher, bleKeyboard) {}

void EncoderModeHandlerVolume::handleRotate(int delta) {
    Serial.printf("EncoderModeHandlerVolume: Rotating by %d\n", delta);

    if (delta > 0) {
        bleKeyboard->write(KEY_MEDIA_VOLUME_UP);

        return;
    }

    bleKeyboard->write(KEY_MEDIA_VOLUME_DOWN);
}

void EncoderModeHandlerVolume::handleShortClick() {
    Serial.println("EncoderModeHandlerVolume: Short click detected.");

    bleKeyboard->write(KEY_MEDIA_MUTE);
}

const char* EncoderModeHandlerVolume::getModeName() const {
    return EncoderModeHelper::toString(EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME);
}
