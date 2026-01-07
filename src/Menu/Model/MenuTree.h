#pragma once

#include "MenuItem.h"
#include "Enum/WheelModeEnum.h"
#include "Menu/Action/SelectWheelModeAction.h"
#include "Menu/Action/SetButtonBehaviorAction.h"
#include "Config/button_config.h"

// Forward declaration
class ButtonEventHandler;

/**
 * @brief Static menu tree structure
 *
 * Defines the complete menu hierarchy with parent-child relationships.
 * Structure uses inline arrays (not constexpr) because:
 * 1. Parent pointers create circular references (C++ constexpr limitation)
 * 2. Action pointers require runtime DI objects (per AC note)
 *
 * Initialization sequence:
 * 1. Call initMenuTree() to set parent pointers
 * 2. Call initWheelBehaviorActions() to set wheel mode action pointers
 * 3. Call initButtonBehaviorActions() to set button action pointers
 *
 * Menu Structure:
 * - Wheel Behavior (branch)
 *   - Wheel Mode (branch)
 *     - Scroll (leaf - SelectWheelModeAction)
 *     - Volume (leaf - SelectWheelModeAction)
 *     - Zoom (leaf - SelectWheelModeAction)
 *   - Wheel Direction (branch)
 *     - Normal (leaf - placeholder, no action)
 *     - Reversed (leaf - placeholder, no action)
 * - Button Config (branch)
 *   - Top Left (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 *   - Top Right (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 *   - Bottom Left (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 *   - Bottom Right (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 * - Device Status (leaf - ShowStatusAction)
 * - About (leaf - ShowAboutAction)
 */
namespace MenuTree {

// Forward declarations for parent references
extern MenuItem mainMenu[];
extern MenuItem wheelBehaviorSubmenu[];
extern MenuItem wheelModeSubmenu[];
extern MenuItem wheelDirectionSubmenu[];
extern MenuItem buttonConfigSubmenu[];
extern MenuItem buttonBehaviorItems[];

// Wheel Mode submenu items (leaf nodes with actions set at runtime)
// Labels derived from WheelMode enum to ensure alignment
inline MenuItem wheelModeSubmenu[] = {
    { wheelModeToDisplayString(WheelMode::SCROLL),  nullptr, nullptr, 0, nullptr },
    { wheelModeToDisplayString(WheelMode::VOLUME),  nullptr, nullptr, 0, nullptr },
    { wheelModeToDisplayString(WheelMode::ZOOM),    nullptr, nullptr, 0, nullptr }
};

inline constexpr uint8_t WHEEL_MODE_COUNT = 3;

// Wheel Direction submenu items (placeholder items for Story 7.2)
inline MenuItem wheelDirectionSubmenu[] = {
    { "Normal",   nullptr, nullptr, 0, nullptr },
    { "Reversed", nullptr, nullptr, 0, nullptr }
};

inline constexpr uint8_t WHEEL_DIRECTION_COUNT = 2;

// Wheel Behavior submenu (branch nodes - parent menu)
inline MenuItem wheelBehaviorSubmenu[] = {
    { "Wheel Mode",      nullptr, wheelModeSubmenu,      WHEEL_MODE_COUNT,      nullptr },
    { "Wheel Direction", nullptr, wheelDirectionSubmenu, WHEEL_DIRECTION_COUNT, nullptr }
};

inline constexpr uint8_t WHEEL_BEHAVIOR_COUNT = 2;

// Shared button behavior items - all buttons have the same 6 options
// Parent pointers set at runtime in initMenuTree() based on which button submenu references this
inline MenuItem buttonBehaviorItems[] = {
    { "None",     nullptr, nullptr, 0, nullptr },
    { "Mute",     nullptr, nullptr, 0, nullptr },
    { "Play",     nullptr, nullptr, 0, nullptr },
    { "Pause",    nullptr, nullptr, 0, nullptr },
    { "Next",     nullptr, nullptr, 0, nullptr },
    { "Previous", nullptr, nullptr, 0, nullptr }
};

inline constexpr uint8_t BUTTON_BEHAVIOR_COUNT = 6;

// Button Config submenu (branch nodes - all reference the same shared buttonBehaviorItems)
// Labels set at runtime from BUTTONS[].label in initMenuTree()
// Array size matches BUTTON_COUNT to prevent overflow if button count changes
inline MenuItem buttonConfigSubmenu[BUTTON_COUNT] = {
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr },
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr },
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr },
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr }
};

// Main menu items
inline MenuItem mainMenu[] = {
    { "Wheel Behavior", nullptr, wheelBehaviorSubmenu, WHEEL_BEHAVIOR_COUNT, nullptr },
    { "Button Config",  nullptr, buttonConfigSubmenu, BUTTON_COUNT, nullptr },
    { "Device Status",  nullptr, nullptr, 0, nullptr },
    { "About",          nullptr, nullptr, 0, nullptr }
};

inline constexpr uint8_t MAIN_MENU_COUNT = 4;

// Root node representing the main menu container
inline MenuItem root = { "Menu", nullptr, mainMenu, MAIN_MENU_COUNT, nullptr };

// Menu item indices for action assignment
namespace Index {
    inline constexpr uint8_t WHEEL_BEHAVIOR = 0;
    inline constexpr uint8_t BUTTON_CONFIG = 1;
    inline constexpr uint8_t DEVICE_STATUS = 2;
    inline constexpr uint8_t ABOUT = 3;
}

/**
 * @brief Initialize parent pointers and button labels in the menu tree
 *
 * Must be called once at startup to establish parent-child relationships
 * and set button labels from button_config.h as single source of truth.
 * Parent pointers cannot be constexpr due to circular reference limitations.
 */
inline void initMenuTree() {
    // Set parent for main menu items
    for (uint8_t i = 0; i < MAIN_MENU_COUNT; i++) {
        mainMenu[i].parent = &root;
    }

    // Set parent for wheel behavior submenu items (Wheel Mode, Wheel Direction)
    for (uint8_t i = 0; i < WHEEL_BEHAVIOR_COUNT; i++) {
        wheelBehaviorSubmenu[i].parent = &mainMenu[Index::WHEEL_BEHAVIOR];
    }

    // Set parent for wheel mode submenu items (Scroll, Volume, Zoom)
    for (uint8_t i = 0; i < WHEEL_MODE_COUNT; i++) {
        wheelModeSubmenu[i].parent = &wheelBehaviorSubmenu[0];  // Parent is "Wheel Mode" item
    }

    // Set parent for wheel direction submenu items (Normal, Reversed)
    for (uint8_t i = 0; i < WHEEL_DIRECTION_COUNT; i++) {
        wheelDirectionSubmenu[i].parent = &wheelBehaviorSubmenu[1];  // Parent is "Wheel Direction" item
    }

    // Set parent and labels for button config submenu items
    // Labels come from BUTTONS[].label (single source of truth)
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        buttonConfigSubmenu[i].parent = &mainMenu[Index::BUTTON_CONFIG];
        buttonConfigSubmenu[i].label = BUTTONS[i].label;
    }

    // Set parent for shared button behavior items
    // Note: Parent is set to buttonConfigSubmenu[0] (Top Left) as default
    // This will be dynamically overridden during navigation in MenuController
    // to reflect which button submenu is currently active
    for (uint8_t i = 0; i < BUTTON_BEHAVIOR_COUNT; i++) {
        buttonBehaviorItems[i].parent = &buttonConfigSubmenu[0];
    }
}

/**
 * @brief Set action for a wheel mode menu item
 * @param index Wheel mode index (0=Scroll, 1=Volume, 2=Zoom)
 * @param action Pointer to MenuAction instance (must outlive menu)
 */
inline void setWheelModeAction(uint8_t index, MenuAction* action) {
    if (index < WHEEL_MODE_COUNT) {
        wheelModeSubmenu[index].action = action;
    }
}



/**
 * @brief Set action for Device Status menu item
 * @param action Pointer to ShowStatusAction instance
 */
inline void setDeviceStatusAction(MenuAction* action) {
    mainMenu[Index::DEVICE_STATUS].action = action;
}

/**
 * @brief Set action for About menu item
 * @param action Pointer to ShowAboutAction instance
 */
inline void setAboutAction(MenuAction* action) {
    mainMenu[Index::ABOUT].action = action;
}

/**
 * @brief Get the root menu item
 * @return Pointer to the root menu item
 */
inline const MenuItem* getRoot() {
    return &root;
}

/**
 * @brief Get the main menu items array
 * @return Pointer to the main menu items array
 */
inline const MenuItem* getMainMenu() {
    return mainMenu;
}

/**
 * @brief Get the main menu item count
 * @return Number of items in the main menu
 */
inline constexpr uint8_t getMainMenuCount() {
    return MAIN_MENU_COUNT;
}

/**
 * @brief Initialize wheel behavior menu actions
 *
 * Creates SelectWheelModeAction instances for Scroll, Volume, and Zoom modes.
 * Must be called after DI objects (ConfigManager, EncoderModeManager) are created.
 *
 * @param config ConfigManager instance for NVS persistence
 * @param modeMgr EncoderModeManager instance for runtime mode switching
 */
inline void initWheelBehaviorActions(ConfigManager* config, EncoderModeManager* modeMgr) {
    // Create static action instances (must outlive menu)
    static SelectWheelModeAction scrollAction(WheelMode::SCROLL, config, modeMgr);
    static SelectWheelModeAction volumeAction(WheelMode::VOLUME, config, modeMgr);
    static SelectWheelModeAction zoomAction(WheelMode::ZOOM, config, modeMgr);

    // Assign actions to wheel mode submenu items
    // Use WheelMode enum values to ensure alignment with menu labels
    setWheelModeAction(static_cast<uint8_t>(WheelMode::SCROLL), &scrollAction);
    setWheelModeAction(static_cast<uint8_t>(WheelMode::VOLUME), &volumeAction);
    setWheelModeAction(static_cast<uint8_t>(WheelMode::ZOOM), &zoomAction);

    // Wheel Direction submenu actions remain nullptr (to be implemented in Story 7.2)
}

/**
 * @brief Initialize button behavior menu actions
 *
 * Creates context-aware SetButtonBehaviorAction instances for shared button behavior menu items.
 * All buttons share the same behavior items array (None, Mute, Play, Pause, Next, Previous).
 * The button index is determined dynamically at runtime by analyzing the menu navigation context
 * (which button submenu the user navigated through).
 * 
 * Must be called after DI objects (ConfigManager, ButtonEventHandler) are created.
 *
 * @param config ConfigManager instance for NVS persistence
 * @param buttonHandler ButtonEventHandler instance for cache invalidation
 */
inline void initButtonBehaviorActions(ConfigManager* config, ButtonEventHandler* buttonHandler) {
    // Button action types in order matching menu items
    static constexpr ButtonAction actions[] = {
        ButtonAction::NONE,
        ButtonAction::MUTE,
        ButtonAction::PLAY,
        ButtonAction::PAUSE,
        ButtonAction::NEXT,
        ButtonAction::PREVIOUS
    };

    // Create static context-aware action instances for the shared button behavior items
    // Button index is extracted from menu context at runtime (no hard-coded button index)
    // buttonHandler enables cache invalidation when config changes
    static SetButtonBehaviorAction buttonActions[BUTTON_BEHAVIOR_COUNT] = {
        SetButtonBehaviorAction(actions[0], config, buttonHandler),
        SetButtonBehaviorAction(actions[1], config, buttonHandler),
        SetButtonBehaviorAction(actions[2], config, buttonHandler),
        SetButtonBehaviorAction(actions[3], config, buttonHandler),
        SetButtonBehaviorAction(actions[4], config, buttonHandler),
        SetButtonBehaviorAction(actions[5], config, buttonHandler)
    };

    // Assign actions to shared button behavior items
    for (uint8_t i = 0; i < BUTTON_BEHAVIOR_COUNT; i++) {
        buttonBehaviorItems[i].action = &buttonActions[i];
    }
}

} // namespace MenuTree
