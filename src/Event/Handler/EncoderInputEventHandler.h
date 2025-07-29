#pragma once

#include "Arduino.h"
#include "Type/EncoderInputEvent.h"
#include "Enum/EventEnum.h"
#include "Helper/EnumToStringHelper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class EncoderInputEventHandler {
public:
    EncoderInputEventHandler(QueueHandle_t queue);

    void start();

private:
    QueueHandle_t eventQueue;

    static void taskEntry(void* param);
    void taskLoop();
};
