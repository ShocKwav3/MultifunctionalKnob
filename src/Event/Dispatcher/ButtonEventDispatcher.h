#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Type/ButtonEvent.h"

class ButtonEventDispatcher {
public:
    explicit ButtonEventDispatcher(QueueHandle_t queue);

    void onButtonShortPress(uint8_t buttonIndex);
    void onButtonLongPress(uint8_t buttonIndex);

private:
    QueueHandle_t eventQueue;
};
