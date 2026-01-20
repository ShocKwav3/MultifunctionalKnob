#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "Enum/PowerStateEnum.h"

class PowerManager {
public:
    PowerManager();

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
     * @brief Set the display request queue for warning display
     * Must be called before start()
     * @param queue Display request queue handle
     */
    void setDisplayQueue(QueueHandle_t queue);

private:
    volatile uint32_t lastActivityTime;  // volatile for thread-safety
    PowerState currentState;
    portMUX_TYPE stateMux = portMUX_INITIALIZER_UNLOCKED;  // Critical section for state access
    bool warningDisplayed;  // Track warning display state
    QueueHandle_t displayQueue;  // Display request queue for warning messages

    static constexpr const char* SLEEP_WARNING_MESSAGE = "Sleep in 1 min";

    static void taskEntry(void* param);
    void taskLoop();
    void updateActivityState();
    void showWarning();
    void clearWarning();
};
