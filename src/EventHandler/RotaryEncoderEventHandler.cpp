#include "RotaryEncoderEventHandler.h"

RotaryEncoderEventHandler::RotaryEncoderEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

void RotaryEncoderEventHandler::onEncoderValueChange(int32_t newValue) {
    int32_t delta = newValue - lastValue;
    lastValue = newValue;

    if (delta != 0 && eventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::ROTATE, delta };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void RotaryEncoderEventHandler::onShortClick() {
    if (eventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::SHORT_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void RotaryEncoderEventHandler::onLongClick() {
    if (eventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::LONG_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}
