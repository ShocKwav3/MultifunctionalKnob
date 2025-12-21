#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Type/MenuEvent.h"
#include "Display/Model/DisplayRequest.h"

/**
 * @brief Handles menu events and translates them to display requests
 *
 * Consumes MenuEvent from MenuEventQueue and pushes DisplayRequest
 * to DisplayRequestQueue. This decouples menu logic from display output.
 *
 * Architecture: Menu Event Pipeline (Option B)
 * MenuController -> MenuEventDispatcher -> MenuEventQueue -> MenuEventHandler -> DisplayRequestQueue
 */
class MenuEventHandler {
public:
    /**
     * @brief Construct MenuEventHandler with required queues
     * @param menuEventQueue Queue to receive MenuEvent from
     * @param displayRequestQueue Queue to send DisplayRequest to
     */
    MenuEventHandler(QueueHandle_t menuEventQueue, QueueHandle_t displayRequestQueue);

    /**
     * @brief Start the event handler task
     * @param stackSize Task stack size in bytes
     * @param priority Task priority
     */
    void start(uint32_t stackSize = 2048, UBaseType_t priority = 1);

private:
    QueueHandle_t menuEventQueue;
    QueueHandle_t displayRequestQueue;

    static void taskEntry(void* param);
    void taskLoop();

    void handleMenuActivated(const MenuEvent& event);
    void handleMenuDeactivated();
    void handleNavigationChanged(const MenuEvent& event);
    void handleItemSelected(const MenuEvent& event);

    void sendDrawMenuRequest(const MenuEvent& event);
    void sendClearRequest();

    static constexpr const char* TAG = "MenuEventHandler";
};
