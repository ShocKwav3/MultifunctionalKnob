#include "EncoderModeHandlerAbstract.h"

EncoderModeHandlerAbstract::EncoderModeHandlerAbstract(AppEventDispatcher* dispatcher)
    : appEventDispatcher(dispatcher) {}

void EncoderModeHandlerAbstract::handleLongClick() {
    Serial.println("AbstractModeHandler: Long click → entering mode selection...");

    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SELECTION);
    }
}

const char* EncoderModeHandlerAbstract::getModeName() const {
    return "AbstractMode";
}
