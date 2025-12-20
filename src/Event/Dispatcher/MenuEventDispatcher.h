#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Type/MenuEvent.h"

/**
 * @brief Singleton dispatcher for menu events
 *
 * MenuController uses this dispatcher to emit events when menu state changes.
 * MenuEventHandler subscribes by consuming events from the queue.
 *
 * Architecture: Distributed Event Pipeline (Option B)
 * MenuController -> MenuEventDispatcher -> MenuEventQueue -> MenuEventHandler
 */
class MenuEventDispatcher {
public:
    /**
     * @brief Initialize the dispatcher with a queue handle
     * @param queue FreeRTOS queue for MenuEvent structs
     */
    static void init(QueueHandle_t queue);

    /**
     * @brief Dispatch a menu activated event
     * @param currentItem The root menu item being displayed
     * @param selectedIndex Initial selection index
     * @param itemCount Number of items in the menu
     */
    static void dispatchActivated(const MenuItem* currentItem, uint8_t selectedIndex, uint8_t itemCount);

    /**
     * @brief Dispatch a menu deactivated event
     */
    static void dispatchDeactivated();

    /**
     * @brief Dispatch a navigation changed event
     * @param currentItem The current menu branch
     * @param selectedIndex New selection index
     * @param itemCount Number of items in the menu
     */
    static void dispatchNavigationChanged(const MenuItem* currentItem, uint8_t selectedIndex, uint8_t itemCount);

    /**
     * @brief Dispatch an item selected event
     * @param selectedItem The menu item that was selected
     */
    static void dispatchItemSelected(const MenuItem* selectedItem);

private:
    static QueueHandle_t eventQueue;

    static void dispatch(const MenuEvent& event);
};
