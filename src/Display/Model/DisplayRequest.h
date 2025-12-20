#pragma once

#include <stdint.h>

/**
 * @brief Maximum number of menu items that can be displayed
 */
static constexpr uint8_t DISPLAY_MAX_MENU_ITEMS = 8;

/**
 * @brief Display request types for the display arbitration queue
 */
enum class DisplayRequestType : uint8_t {
    DRAW_MENU,    ///< Draw a menu with items and selection indicator
    SHOW_STATUS,  ///< Show a key-value status pair
    SHOW_MESSAGE, ///< Show a simple message
    CLEAR         ///< Clear the display
};

/**
 * @brief Display request for the display arbitration queue
 *
 * All display output goes through DisplayRequestQueue to DisplayTask,
 * which arbitrates access to the shared display hardware.
 *
 * Uses a union to minimize memory footprint while supporting
 * different request types.
 */
struct DisplayRequest {
    DisplayRequestType type;

    union {
        struct {
            const char* title;                          ///< Menu title
            const char* items[DISPLAY_MAX_MENU_ITEMS];  ///< Array of menu item labels
            uint8_t count;                              ///< Number of items
            uint8_t selected;                           ///< Currently selected index
        } menu;

        struct {
            const char* key;    ///< Status key/label
            const char* value;  ///< Status value
        } status;

        struct {
            const char* message;  ///< Message text to display
        } message;
    } data;
};
