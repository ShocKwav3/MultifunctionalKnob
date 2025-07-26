#include "EncoderModeSelector.h"

EncoderModeSelector::EncoderModeSelector(AppEventDispatcher* dispatcher)
    : appEventDispatcher(dispatcher) {}

void EncoderModeSelector::handleRotate(int delta) {
    int modeCount = static_cast<int>(EventEnum::EncoderModeEventTypes::__ENCODER_MODE_SELECTION_LIMIT);

    selectionIndex = (selectionIndex + delta) % modeCount;
    if (selectionIndex < 0) selectionIndex += modeCount;

    Serial.print("ModeSelection: Selected = ");
    Serial.println(EncoderModeHelper::toString(static_cast<EventEnum::EncoderModeEventTypes>(selectionIndex)));
}

void EncoderModeSelector::handleShortClick() {
    if (appEventDispatcher) {
        EventEnum::EncoderModeEventTypes selectedMode = static_cast<EventEnum::EncoderModeEventTypes>(selectionIndex);
        appEventDispatcher->dispatchAppEvent(selectedMode);
    }
}

void EncoderModeSelector::handleLongClick() {
    if (appEventDispatcher) {
        appEventDispatcher->dispatchAppEvent(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION_CANCELLED);
    }
}

const char* EncoderModeSelector::getModeName() const {
    return EncoderModeHelper::toString(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION);
}
