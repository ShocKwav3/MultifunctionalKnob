#pragma once

#include <stdint.h>

// Forward declaration to avoid circular dependency
struct HardwareState;

/**
 * @brief Abstract interface for display output
 *
 * Provides a hardware-agnostic interface for displaying menus, messages,
 * confirmations, and status information. Implementations can target
 * serial output, OLED displays, or other output devices.
 */
class DisplayInterface {
public:
    virtual ~DisplayInterface() = default;

    /**
     * @brief Display a menu with selectable items
     * @param title Menu title to display
     * @param items Array of menu item strings
     * @param count Number of items in the array
     * @param selected Index of the currently selected item (0-based)
     */
    virtual void showMenu(const char* title, const char* const* items, uint8_t count, uint8_t selected) = 0;

    /**
     * @brief Display a general message
     * @param message The message text to display
     */
    virtual void showMessage(const char* message) = 0;

    /**
     * @brief Display a confirmation message
     * @param message The confirmation text to display
     */
    virtual void showConfirmation(const char* message) = 0;

    /**
     * @brief Display a key-value status pair
     * @param key The status key/label
     * @param value The status value
     */
    virtual void showStatus(const char* key, const char* value) = 0;

    /**
     * @brief Clear the display
     */
    virtual void clear() = 0;

    /**
     * @brief Draw normal mode status screen with icons
     * @param state Hardware state containing mode, direction, battery, BT status
     */
    virtual void drawNormalMode(const HardwareState& state) = 0;
};
