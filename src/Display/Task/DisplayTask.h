#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "../Interface/DisplayInterface.h"
#include "../Model/DisplayRequest.h"

/**
 * @brief FreeRTOS task that arbitrates display access
 *
 * Consumes DisplayRequest from a queue and routes to DisplayInterface.
 * All display output must go through this task to prevent race conditions
 * on the shared display hardware.
 *
 * Architecture: Display Arbitration Pattern
 * *Handler -> DisplayRequestQueue -> DisplayTask -> DisplayInterface
 */
class DisplayTask {
public:
    /**
     * @brief Construct DisplayTask with display interface
     * @param display Pointer to DisplayInterface implementation (must outlive task)
     */
    explicit DisplayTask(DisplayInterface* display);

    /**
     * @brief Initialize the display request queue
     * @param queueSize Number of requests the queue can hold
     * @return Queue handle for producers to use
     */
    QueueHandle_t init(uint8_t queueSize = 10);

    /**
     * @brief Start the display task
     * @param stackSize Task stack size in bytes
     * @param priority Task priority
     * @return true if task started successfully
     */
    bool start(uint32_t stackSize = 2048, UBaseType_t priority = 1);

    /**
     * @brief Get the display request queue handle
     * @return Queue handle for sending DisplayRequest
     */
    QueueHandle_t getQueue() const;

private:
    DisplayInterface* display;
    QueueHandle_t requestQueue;
    TaskHandle_t taskHandle;

    static void taskFunction(void* params);
    void processRequest(const DisplayRequest& request);

    static constexpr const char* TAG = "DisplayTask";
};
