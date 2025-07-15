#pragma once

#include "Type/RotaryEncoderEventType.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "../../Mode/Handler/ModeHandlerInterface.h"

class RotaryEncoderEventHandler {
public:
    RotaryEncoderEventHandler(QueueHandle_t queue);

    void setModeHandler(ModeHandlerInterface* handler);
    void start();  // creates the FreeRTOS task

private:
    QueueHandle_t eventQueue;
    ModeHandlerInterface* currentHandler = nullptr;

    static void taskEntry(void* param); // FreeRTOS static entry
    void taskLoop();                    // actual loop
};
