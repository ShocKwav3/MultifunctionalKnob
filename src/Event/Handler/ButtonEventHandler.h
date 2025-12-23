#pragma once

#include "Arduino.h"
#include "Type/ButtonEvent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class ButtonEventHandler {
public:
    explicit ButtonEventHandler(QueueHandle_t queue);

    void start();

private:
    QueueHandle_t eventQueue;

    static void taskEntry(void* param);
    void taskLoop();
};
