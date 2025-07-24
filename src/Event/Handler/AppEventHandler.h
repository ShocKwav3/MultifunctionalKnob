#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Arduino.h"

#include "Enum/EventEnum.h"
#include "Type/AppEvent.h"
#include "EncoderMode/Manager/EncoderModeManager.h"
#include "Helper/EncoderModeHelper.h"

class EncoderModeManager;

class AppEventHandler {
public:
    AppEventHandler(QueueHandle_t queue, EncoderModeManager* modeManager);

    void start();

private:
    QueueHandle_t eventQueue;
    EncoderModeManager* encoderModeManager;

    static void taskEntry(void* param);
    void taskLoop();
};
