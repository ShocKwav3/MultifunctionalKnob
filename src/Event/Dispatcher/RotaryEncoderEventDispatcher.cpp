#include "RotaryEncoderEventDispatcher.h"

RotaryEncoderEventDispatcher::RotaryEncoderEventDispatcher(QueueHandle_t queue)
    : eventQueue(queue) {}

void RotaryEncoderEventDispatcher::onEncoderValueChange(int32_t newValue) {
    int32_t delta = newValue - lastValue;
    lastValue = newValue;

    if (delta != 0 && eventQueue) {
        RotaryEncoderEventType evt{ EventEnum::RotaryEncoderEventTypes::ROTATE, delta };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void RotaryEncoderEventDispatcher::onShortClick() {
    if (eventQueue) {
        RotaryEncoderEventType evt{ EventEnum::RotaryEncoderEventTypes::SHORT_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void RotaryEncoderEventDispatcher::onLongClick() {
    if (eventQueue) {
        RotaryEncoderEventType evt{ EventEnum::RotaryEncoderEventTypes::LONG_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}
