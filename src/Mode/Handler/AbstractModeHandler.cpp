#include "AbstractModeHandler.h"

AbstractModeHandler::AbstractModeHandler(AppEventDispatcher* dispatcher)
    : appEventDispatcher(dispatcher) {}

void AbstractModeHandler::handleLongClick() {
    Serial.println("AbstractModeHandler: Long click → entering mode selection...");

    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(EventEnum::AppEventTypes::MODE_SELECTION);
    }
}

const char* AbstractModeHandler::getModeName() const {
    return "AbstractMode";
}
