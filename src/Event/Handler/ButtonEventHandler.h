#pragma once

#include "Arduino.h"
#include "Type/ButtonEvent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "BleKeyboard.h"
#include "Enum/ButtonActionEnum.h"

// Forward declarations
class ConfigManager;

class ButtonEventHandler {
public:
    /**
     * @brief Constructor with dependency injection
     * @param queue Event queue for button events
     * @param config ConfigManager instance to read button action configuration
     * @param keyboard BleKeyboard instance to send HID commands
     */
    ButtonEventHandler(QueueHandle_t queue, ConfigManager* config, BleKeyboard* keyboard);

    void start();
    
    /**
     * @brief Invalidate the button action cache, forcing reload from NVS on next button press
     * Called by SetButtonBehaviorAction when configuration changes
     */
    void invalidateCache();

private:
    QueueHandle_t eventQueue;
    ConfigManager* configManager;
    BleKeyboard* bleKeyboard;
    
    // RAM cache for button actions (avoid NVS read latency on every button press)
    ButtonAction actionCache[4];  // BUTTON_COUNT = 4 from button_config.h
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
