#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Arduino.h"

#include "Enum/EventEnum.h"
#include "Type/AppEvent.h"
#include "Mode/Manager/ModeManager.h"
#include "Helper/ModeHelper.h"

class ModeManager;

class AppEventHandler {
public:
    AppEventHandler(QueueHandle_t queue, ModeManager* modeManager);

    void start();

private:
    QueueHandle_t eventQueue;
    ModeManager* modeManager;

    static void taskEntry(void* param);
    void taskLoop();
};
