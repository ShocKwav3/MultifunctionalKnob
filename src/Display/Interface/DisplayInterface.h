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
     * @param hwState Hardware state for menu status bar (BT, mode, battery)
     */
    virtual void showMenu(const char* title, const char* const* items, uint8_t count, uint8_t selected, const HardwareState& hwState) = 0;

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
     * @param hwState Hardware state containing mode, direction, battery, BT status
     */
    virtual void drawNormalMode(const HardwareState& hwState) = 0;

    /**
     * @brief Control display power state
     * @param on true to turn display on, false to enter power-save mode
     *
     * Implementations should use hardware-specific power commands (e.g.,
     * SSD1306_DISPLAYON/DISPLAYOFF for OLED displays) to control the
     * display power state. Display functionality should remain operational
     * even when powered off (i.e., buffer updates continue, only output
     * is disabled).
     */
    virtual void setPower(bool on) = 0;
};
