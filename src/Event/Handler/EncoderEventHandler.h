#pragma once

#include "Arduino.h"
#include "Type/EncoderInputEvent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "EncoderMode/Handler/EncoderModeHandlerInterface.h"

class EncoderEventHandler {
public:
    EncoderEventHandler(QueueHandle_t queue);

    void setModeHandler(EncoderModeHandlerInterface* handler);
    void start();  // creates the FreeRTOS task

private:
    QueueHandle_t eventQueue;
    EncoderModeHandlerInterface* currentHandler = nullptr;

    static void taskEntry(void* param); // FreeRTOS static entry
    void taskLoop();                    // actual loop
};
