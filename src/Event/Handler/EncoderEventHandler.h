#pragma once

#include "Arduino.h"
#include "Type/EncoderInputEvent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "EncoderMode/Handler/EncoderModeHandlerInterface.h"
#include "EncoderMode/Interface/EncoderModeBaseInterface.h"

class EncoderEventHandler {
public:
    EncoderEventHandler(QueueHandle_t queue);

    void setModeHandler(EncoderModeBaseInterface* handler);
    void start();

private:
    QueueHandle_t eventQueue;
    EncoderModeBaseInterface* currentHandler = nullptr;

    static void taskEntry(void* param);
    void taskLoop();
};
