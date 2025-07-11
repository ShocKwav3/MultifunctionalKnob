#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Event/RotaryEncoderEvent.h"

class RotaryEncoderEventHandler {
public:
    static void setQueue(QueueHandle_t queue);

    static void onEncoderValueChange(int32_t newValue);
    static void onShortClick();
    static void onLongClick();

private:
    static QueueHandle_t rotaryEventQueue;
};
