#include "EncoderEventDispatcher.h"
#include "Config/ConfigManager.h"
#include "Enum/WheelDirection.h"

EncoderEventDispatcher::EncoderEventDispatcher(QueueHandle_t queue, ConfigManager* configManager)
    : eventQueue(queue), configManager(configManager) {}

void EncoderEventDispatcher::onEncoderValueChange(int32_t newValue) {
    // Encoder boundaries: 0-1000 with circular mode enabled
    constexpr int32_t MIN_VALUE = 0;
    constexpr int32_t MAX_VALUE = 1000;
    constexpr int32_t RANGE = MAX_VALUE - MIN_VALUE;
    constexpr int32_t WRAP_THRESHOLD = RANGE / 2;  // 500 - detect wrap if delta > this
    
    int32_t delta = newValue - lastValue;
    
    // Detect and correct wrap-around
    if (delta > WRAP_THRESHOLD) {
        // Wrapped backward: 5 -> 998 should be -7, not +993
        delta = delta - RANGE;
    } else if (delta < -WRAP_THRESHOLD) {
        // Wrapped forward: 998 -> 5 should be +7, not -993
        delta = delta + RANGE;
    }
    
    // Apply wheel direction inversion if configured
    if (configManager && configManager->getWheelDirection() == WheelDirection::REVERSED) {
        delta = -delta;
    }
    
    lastValue = newValue;

    if (delta != 0 && eventQueue) {
        EncoderInputEvent evt{ EventEnum::EncoderInputEventTypes::ROTATE, delta };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void EncoderEventDispatcher::onShortClick() {
    if (eventQueue) {
        EncoderInputEvent evt{ EventEnum::EncoderInputEventTypes::SHORT_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}

void EncoderEventDispatcher::onLongClick() {
    if (eventQueue) {
        EncoderInputEvent evt{ EventEnum::EncoderInputEventTypes::LONG_CLICK };
        xQueueSend(eventQueue, &evt, 0);
    }
}
