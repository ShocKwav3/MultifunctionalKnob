#include "AbstractModeHandler.h"
#include "Enum/ModeEnum.h"

AbstractModeHandler::AbstractModeHandler(AppEventDispatcher* dispatcher)
    : appEventDispatcher(dispatcher) {}

void AbstractModeHandler::handleLongClick() {
    Serial.println("AbstractModeHandler: Long click → entering mode selection...");

    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(ModeEnum::AppEventTypes::MODE_SELECTION);
    }
}

const char* AbstractModeHandler::getModeName() const {
    return "AbstractMode";
}
