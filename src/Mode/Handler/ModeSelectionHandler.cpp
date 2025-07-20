#include "ModeSelectionHandler.h"

using namespace ModeEnum;

ModeSelectionHandler::ModeSelectionHandler(AppEventDispatcher* dispatcher)
    : AbstractModeHandler(dispatcher) {}

void ModeSelectionHandler::handleRotate(int delta) {
    int modeCount = static_cast<int>(ModeEnum::AppEventTypes::__MODE_SELECTION_LIMIT);

    selectionIndex = (selectionIndex + delta) % modeCount;
    if (selectionIndex < 0) selectionIndex += modeCount;

    Serial.print("ModeSelection: Selected = ");
    Serial.println(ModeHelper::toString(static_cast<ModeEnum::AppEventTypes>(selectionIndex)));
}

void ModeSelectionHandler::handleShortClick() {
    if (appEventDispatcher) {
        ModeEnum::AppEventTypes selectedMode = static_cast<ModeEnum::AppEventTypes>(selectionIndex);
        appEventDispatcher->dispatchAppEvent(selectedMode);
    }
}

void ModeSelectionHandler::handleLongClick() {
    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(ModeEnum::AppEventTypes::MODE_SELECTION_CANCELLED);
    }
}

const char* ModeSelectionHandler::getModeName() const {
    return ModeHelper::toString(AppEventTypes::MODE_SELECTION);
}
