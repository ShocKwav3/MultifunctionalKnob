#include "EncoderModeSelectionHandler.h"

EncoderModeSelectionHandler::EncoderModeSelectionHandler(AppEventDispatcher* dispatcher)
    : EncoderModeHandlerAbstract(dispatcher) {}

void EncoderModeSelectionHandler::handleRotate(int delta) {
    int modeCount = static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT);

    selectionIndex = (selectionIndex + delta) % modeCount;
    if (selectionIndex < 0) selectionIndex += modeCount;

    Serial.print("ModeSelection: Selected = ");
    Serial.println(EncoderModeHelper::toString(static_cast<EventEnum::AppEventTypes>(selectionIndex)));
}

void EncoderModeSelectionHandler::handleShortClick() {
    if (appEventDispatcher) {
        EventEnum::AppEventTypes selectedMode = static_cast<EventEnum::AppEventTypes>(selectionIndex);
        appEventDispatcher->dispatchAppEvent(selectedMode);
    }
}

void EncoderModeSelectionHandler::handleLongClick() {
    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(EventEnum::AppEventTypes::MODE_SELECTION_CANCELLED);
    }
}

const char* EncoderModeSelectionHandler::getModeName() const {
    return EncoderModeHelper::toString(EventEnum::AppEventTypes::MODE_SELECTION);
}
