#pragma once

#include "MenuItem.h"
#include "Menu/Action/SelectWheelModeAction.h"
#include "Menu/Action/SetButtonBehaviorAction.h"

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
 *   - Scroll (leaf - SelectWheelModeAction)
 *   - Volume (leaf - SelectWheelModeAction)
 *   - Zoom (leaf - SelectWheelModeAction)
 * - Button Config (branch)
 *   - Button 1 (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 *   - Button 2 (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 *   - Button 3 (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 *   - Button 4 (branch)
 *     - None, Mute, Play, Pause, Next, Previous (leaf - SetButtonBehaviorAction)
 * - Device Status (leaf - ShowStatusAction)
 * - About (leaf - ShowAboutAction)
 */
namespace MenuTree {

// Forward declarations for parent references
extern MenuItem mainMenu[];
extern MenuItem wheelBehaviorSubmenu[];
extern MenuItem buttonConfigSubmenu[];
extern MenuItem buttonBehaviorItems[];

// Wheel Behavior submenu items (leaf nodes with actions set at runtime)
inline MenuItem wheelBehaviorSubmenu[] = {
    { "Scroll",  nullptr, nullptr, 0, nullptr },
    { "Volume",  nullptr, nullptr, 0, nullptr },
    { "Zoom",    nullptr, nullptr, 0, nullptr }
};

inline constexpr uint8_t WHEEL_BEHAVIOR_COUNT = 3;

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
inline MenuItem buttonConfigSubmenu[] = {
    { "Button 1", nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr },
    { "Button 2", nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr },
    { "Button 3", nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr },
    { "Button 4", nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr }
};

inline constexpr uint8_t BUTTON_CONFIG_COUNT = 4;

// Main menu items
inline MenuItem mainMenu[] = {
    { "Wheel Behavior", nullptr, wheelBehaviorSubmenu, WHEEL_BEHAVIOR_COUNT, nullptr },
    { "Button Config",  nullptr, buttonConfigSubmenu, BUTTON_CONFIG_COUNT, nullptr },
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
 * @brief Initialize parent pointers in the menu tree
 *
 * Must be called once at startup to establish parent-child relationships.
 * Parent pointers cannot be constexpr due to circular reference limitations.
 */
inline void initMenuTree() {
    // Set parent for main menu items
    for (uint8_t i = 0; i < MAIN_MENU_COUNT; i++) {
        mainMenu[i].parent = &root;
    }

    // Set parent for wheel behavior submenu items
    for (uint8_t i = 0; i < WHEEL_BEHAVIOR_COUNT; i++) {
        wheelBehaviorSubmenu[i].parent = &mainMenu[Index::WHEEL_BEHAVIOR];
    }

    // Set parent for button config submenu items (Button 1-4)
    for (uint8_t i = 0; i < BUTTON_CONFIG_COUNT; i++) {
        buttonConfigSubmenu[i].parent = &mainMenu[Index::BUTTON_CONFIG];
    }

    // Set parent for shared button behavior items
    // Note: Parent is set to buttonConfigSubmenu[0] (Button 1) as default
    // This will be dynamically overridden during navigation in MenuController
    // to reflect which button submenu is currently active
    for (uint8_t i = 0; i < BUTTON_BEHAVIOR_COUNT; i++) {
        buttonBehaviorItems[i].parent = &buttonConfigSubmenu[0];
    }
}

/**
 * @brief Set action for a wheel behavior menu item
 * @param index Wheel behavior index (0=Scroll, 1=Volume, 2=Zoom)
 * @param action Pointer to MenuAction instance (must outlive menu)
 */
inline void setWheelBehaviorAction(uint8_t index, MenuAction* action) {
    if (index < WHEEL_BEHAVIOR_COUNT) {
        wheelBehaviorSubmenu[index].action = action;
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

    // Assign actions to menu items
    setWheelBehaviorAction(0, &scrollAction);   // Scroll
    setWheelBehaviorAction(1, &volumeAction);   // Volume
    setWheelBehaviorAction(2, &zoomAction);     // Zoom
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
