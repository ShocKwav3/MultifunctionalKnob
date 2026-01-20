#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "Enum/PowerStateEnum.h"
#include "BleKeyboard.h"

// Forward declarations
class DisplayInterface;

class PowerManager {
public:
    /**
     * @brief Construct PowerManager with required dependencies
     * @param keyboard BLE keyboard for cleanup before sleep
     * @param display Display interface for cleanup before sleep
     * @param displayQueue Display request queue for warning messages
     */
    PowerManager(BleKeyboard& keyboard, DisplayInterface& display, QueueHandle_t displayQueue);

    // Prevent copying (task/mutex duplication hazard)
    PowerManager(const PowerManager&) = delete;
    PowerManager& operator=(const PowerManager&) = delete;

    /**
     * @brief Reset activity timer (called by input handlers)
     * Thread-safe: Can be called from multiple tasks
     */
    void resetActivity();

    /**
     * @brief Get current power state
     * Thread-safe: Can be called from any task
     * @return Current PowerState
     */
    PowerState getState() const;

    /**
     * @brief Start the power manager task
     * Creates FreeRTOS task that monitors inactivity
     */
    void start();

    /**
     * @brief Enter deep sleep mode
     * Performs cleanup (display, BLE) then enters ESP32 deep sleep
     * Device will wake only from configured wake sources
     */
    void enterDeepSleep();

private:
    volatile uint32_t lastActivityTime;  // volatile for thread-safety
    PowerState currentState;
    portMUX_TYPE stateMux = portMUX_INITIALIZER_UNLOCKED;  // Critical section for state access
    bool warningDisplayed;  // Track warning display state
    QueueHandle_t displayQueue;  // Display request queue for warning messages
    BleKeyboard& bleKeyboard;  // BLE keyboard for cleanup before sleep
    DisplayInterface& display;  // Display interface for cleanup before sleep

    static constexpr const char* SLEEP_WARNING_MESSAGE = "Sleep in 1 min";

    static void taskEntry(void* param);
    void taskLoop();
    void updateActivityState();
    void showWarning();
    void clearWarning();
};
