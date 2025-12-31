#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Type/EncoderInputEvent.h"

class EncoderEventDispatcher {
public:
    EncoderEventDispatcher(QueueHandle_t queue);

    void onEncoderValueChange(int32_t newValue);
    void onShortClick();
    void onLongClick();

private:
    QueueHandle_t eventQueue;
    int32_t lastValue = 500;  // Match encoder initial value from EncoderDriver::begin()
};
