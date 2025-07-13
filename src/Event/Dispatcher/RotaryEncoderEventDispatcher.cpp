#include "RotaryEncoderEventDispatcher.h"

RotaryEncoderEventDispatcher::RotaryEncoderEventDispatcher(QueueHandle_t queue)
    : eventQueue(queue) {}

void RotaryEncoderEventDispatcher::onEncoderValueChange(int32_t newValue) {
    int32_t delta = newValue - lastValue;
    lastValue = newValue;

    if (delta != 0 && eventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::ROTATE, delta };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void RotaryEncoderEventDispatcher::onShortClick() {
    if (eventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::SHORT_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void RotaryEncoderEventDispatcher::onLongClick() {
    if (eventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::LONG_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}
