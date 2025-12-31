#pragma once

#include <stdint.h>
#include "../Model/MenuItem.h"

/**
 * @brief Menu controller state machine
 *
 * Manages menu navigation state and emits events via MenuEventDispatcher.
 * Tracks active state, current menu position, and selection index.
 *
 * Input events (rotation, select, back) are routed here when menu is active.
 * State changes emit MenuEvent via MenuEventDispatcher for display updates.
 */
class MenuController {
public:
    MenuController();

    /**
     * @brief Check if menu system is currently active
     * @return true if menu is active and consuming input
     */
    bool isActive() const;

    /**
     * @brief Activate the menu system
     *
     * Resets state to root menu, emits MENU_ACTIVATED event.
     */
    void activate();

    /**
     * @brief Deactivate the menu system
     *
     * Clears state, emits MENU_DEACTIVATED event.
     */
    void deactivate();

    /**
     * @brief Handle encoder rotation
     * @param delta Rotation direction (+1 = clockwise, -1 = counter-clockwise)
     *
     * Updates selection index within bounds, emits MENU_NAVIGATION_CHANGED.
     */
    void handleRotation(int8_t delta);

    /**
     * @brief Handle select action (short click)
     *
     * If current item has children: enters submenu.
     * If current item has action: executes action and emits MENU_ITEM_SELECTED.
     */
    void handleSelect();

    /**
     * @brief Handle back action (long click)
     *
     * If at root: deactivates menu (emits MENU_DEACTIVATED).
     * If in submenu: navigates to parent menu (emits MENU_NAVIGATION_CHANGED).
     */
    void handleBack();

    /**
     * @brief Get current menu item (branch node being displayed)
     * @return Pointer to current menu item, or nullptr if inactive
     */
    const MenuItem* getCurrentItem() const;

    /**
     * @brief Get current selection index
     * @return Index of selected item within current menu
     */
    uint8_t getSelectedIndex() const;

private:
    bool active;
    bool viewing;                 ///< True when viewing a leaf screen (Device Status, About)
    const MenuItem* currentItem;  ///< Current menu branch (parent of visible items)
    uint8_t selectedIndex;        ///< Index of selected child

    /**
     * @brief Check if menu can respond to navigation (rotation)
     * @return true if active, not viewing, and has navigable items
     */
    bool canNavigate() const;

    /**
     * @brief Check if menu can respond to selection (short click)
     * @return true if active, not viewing, and has selectable items
     */
    bool canSelect() const;

    void emitNavigationChanged();
    void emitActivated();
};
