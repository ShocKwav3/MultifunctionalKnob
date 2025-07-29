#include "EncoderInputEventDispatcher.h"

EncoderInputEventDispatcher::EncoderInputEventDispatcher(QueueHandle_t queue)
    : eventQueue(queue) {}

void EncoderInputEventDispatcher::onEncoderValueChange(int32_t newValue) {
    int32_t delta = newValue - lastValue;
    lastValue = newValue;

    if (delta != 0 && eventQueue) {
        EncoderInputEvent evt{ EventEnum::EncoderInputEventTypes::ROTATE, delta };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void EncoderInputEventDispatcher::onShortClick() {
    if (eventQueue) {
        EncoderInputEvent evt{ EventEnum::EncoderInputEventTypes::SHORT_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void EncoderInputEventDispatcher::onLongClick() {
    if (eventQueue) {
        EncoderInputEvent evt{ EventEnum::EncoderInputEventTypes::LONG_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}
