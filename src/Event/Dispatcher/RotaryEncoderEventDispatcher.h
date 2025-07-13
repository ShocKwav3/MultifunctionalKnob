#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Event/RotaryEncoderEvent.h"

class RotaryEncoderEventDispatcher {
public:
    RotaryEncoderEventDispatcher(QueueHandle_t queue);

    void onEncoderValueChange(int32_t newValue);
    void onShortClick();
    void onLongClick();

private:
    QueueHandle_t eventQueue;
    int32_t lastValue = 0;
};
