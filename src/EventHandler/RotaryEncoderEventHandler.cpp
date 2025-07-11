#include "RotaryEncoderEventHandler.h"

QueueHandle_t RotaryEncoderEventHandler::rotaryEventQueue = nullptr;

void RotaryEncoderEventHandler::setQueue(QueueHandle_t queue) {
    rotaryEventQueue = queue;
}

void RotaryEncoderEventHandler::onEncoderValueChange(int32_t newValue) {
    static int32_t lastValue = 0;
    int32_t delta = newValue - lastValue;
    lastValue = newValue;

    if (delta != 0 && rotaryEventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::ROTATE, delta };

        xQueueSend(rotaryEventQueue, &evt, 0);
    }
}

void RotaryEncoderEventHandler::onShortClick() {
    if (rotaryEventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::SHORT_CLICK };

        xQueueSend(rotaryEventQueue, &evt, 0);
    }
}

void RotaryEncoderEventHandler::onLongClick() {
    if (rotaryEventQueue) {
        RotaryEncoderEvent evt{ EventEnum::RotaryEncoderEventType::LONG_CLICK };

        xQueueSend(rotaryEventQueue, &evt, 0);
    }
}
