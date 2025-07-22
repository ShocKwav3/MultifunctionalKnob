#include "ModeSelectionHandler.h"

ModeSelectionHandler::ModeSelectionHandler(AppEventDispatcher* dispatcher)
    : AbstractModeHandler(dispatcher) {}

void ModeSelectionHandler::handleRotate(int delta) {
    int modeCount = static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT);

    selectionIndex = (selectionIndex + delta) % modeCount;
    if (selectionIndex < 0) selectionIndex += modeCount;

    Serial.print("ModeSelection: Selected = ");
    Serial.println(ModeHelper::toString(static_cast<EventEnum::AppEventTypes>(selectionIndex)));
}

void ModeSelectionHandler::handleShortClick() {
    if (appEventDispatcher) {
        EventEnum::AppEventTypes selectedMode = static_cast<EventEnum::AppEventTypes>(selectionIndex);
        appEventDispatcher->dispatchAppEvent(selectedMode);
    }
}

void ModeSelectionHandler::handleLongClick() {
    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(EventEnum::AppEventTypes::MODE_SELECTION_CANCELLED);
    }
}

const char* ModeSelectionHandler::getModeName() const {
    return ModeHelper::toString(EventEnum::AppEventTypes::MODE_SELECTION);
}
