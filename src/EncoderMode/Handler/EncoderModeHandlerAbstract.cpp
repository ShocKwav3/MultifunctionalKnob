#include "EncoderModeHandlerAbstract.h"

EncoderModeHandlerAbstract::EncoderModeHandlerAbstract(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard)
    : appEventDispatcher(dispatcher), bleKeyboard(bleKeyboard) {}

void EncoderModeHandlerAbstract::handleLongClick() {
    Serial.println("AbstractModeHandler: Long click → entering mode selection...");

    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION);
    }
}

const char* EncoderModeHandlerAbstract::getModeName() const {
    return "AbstractMode";
}
