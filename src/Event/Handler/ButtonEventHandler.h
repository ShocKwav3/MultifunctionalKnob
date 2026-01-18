#pragma once

#include "Arduino.h"
#include "Type/ButtonEvent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "BleKeyboard.h"
#include "Config/button_config.h"
#include "Enum/ButtonActionEnum.h"
#include "Event/Handler/Interface/EventHandlerInterface.h"

// Forward declarations
class ConfigManager;
class PowerManager;

class ButtonEventHandler : public EventHandlerInterface {
public:
    /**
     * @brief Constructor with dependency injection
     * @param queue Event queue for button events
     * @param config ConfigManager instance to read button action configuration
     * @param keyboard BleKeyboard instance to send HID commands
     * @param pm PowerManager instance to reset activity timer
     */
    ButtonEventHandler(QueueHandle_t queue, ConfigManager* config, BleKeyboard* keyboard, PowerManager* pm);

    void start();

    /**
     * @brief Invalidate the button action cache, forcing reload from NVS on next button press
     * Called by SetButtonBehaviorAction when configuration changes
     */
    void invalidateCache();

    // EventHandlerInterface implementation
    void notifyUserActivity() override;

private:
    QueueHandle_t eventQueue;
    ConfigManager* configManager;
    BleKeyboard* bleKeyboard;
    PowerManager* powerManager;

    // RAM cache for button actions (avoid NVS read latency on every button press)
    // Size automatically syncs with BUTTON_COUNT from button_config.h (compile-time constant)
    ButtonAction actionCache[BUTTON_COUNT];
    bool cacheValid;

    static void taskEntry(void* param);
    void taskLoop();
    
    /**
     * @brief Load button action configuration from NVS into RAM cache
     */
    void loadCache();
    
    /**
     * @brief Execute the configured action for a button press
     * @param buttonIndex Index of the button (0 to BUTTON_COUNT-1)
     */
    void executeButtonAction(uint8_t buttonIndex);
};
