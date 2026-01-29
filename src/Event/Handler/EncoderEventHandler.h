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
class MacroManager;
struct HardwareState;

class EncoderEventHandler : public EventHandlerInterface {
public:
    EncoderEventHandler(QueueHandle_t queue, PowerManager* pm, HardwareState* hwState, MacroManager* macroMgr);

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
    HardwareState* hardwareState;
    MacroManager* macroManager;

    static void taskEntry(void* param);
    void taskLoop();

    /**
     * @brief Convert EncoderInputEvent to MacroInput enum
     * @param evt Encoder event to convert
     * @return Corresponding MacroInput value
     */
    static uint8_t mapEncoderEventToMacroInput(const EncoderInputEvent& evt);
};
