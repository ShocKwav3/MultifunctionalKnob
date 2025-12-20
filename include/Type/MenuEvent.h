#pragma once

#include <stdint.h>

class MenuItem;

/**
 * @brief Menu event types for the menu event pipeline
 *
 * These events are emitted by MenuController and consumed by MenuEventHandler.
 */
enum class MenuEventType : uint8_t {
    MENU_ACTIVATED,           ///< Menu system was activated
    MENU_DEACTIVATED,         ///< Menu system was deactivated
    MENU_NAVIGATION_CHANGED,  ///< Selection index changed within current menu
    MENU_ITEM_SELECTED        ///< A menu item was selected (action executed or submenu entered)
};

/**
 * @brief Menu event data for the menu event pipeline
 *
 * Contains all information needed to update the display based on menu state changes.
 */
struct MenuEvent {
    MenuEventType type;              ///< Type of menu event
    const MenuItem* currentItem;     ///< Current menu item context (branch node for navigation)
    uint8_t selectedIndex;           ///< Currently selected index within the menu
    uint8_t itemCount;               ///< Total number of items in current menu
};
