#pragma once

#include "Arduino.h"
#include "Type/EncoderInputEvent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "EncoderMode/Handler/EncoderModeHandlerInterface.h"
#include "EncoderMode/Interface/EncoderModeBaseInterface.h"
#include "Event/Handler/Interface/EventHandlerInterface.h"

class MenuController;
class PowerManager;

class EncoderEventHandler : public EventHandlerInterface {
public:
    EncoderEventHandler(QueueHandle_t queue, PowerManager* pm);

    void setModeHandler(EncoderModeBaseInterface* handler);
    void setMenuController(MenuController* controller);
    void start();

    // EventHandlerInterface implementation
    void notifyUserActivity() override;

private:
    QueueHandle_t eventQueue;
    EncoderModeBaseInterface* currentHandler = nullptr;
    MenuController* menuController = nullptr;
    PowerManager* powerManager;

    static void taskEntry(void* param);
    void taskLoop();
};
