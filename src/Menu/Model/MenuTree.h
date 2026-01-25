#pragma once

#include "MenuItem.h"
#include "Enum/WheelModeEnum.h"
#include "Enum/WheelDirection.h"
#include "Menu/Action/SelectWheelModeAction.h"
#include "Menu/Action/SelectWheelDirectionAction.h"
#include "Menu/Action/SetButtonBehaviorAction.h"
#include "Menu/Action/PairAction.h"
#include "Menu/Action/DisconnectAction.h"
#include "Menu/Action/DisplayPowerAction.h"
#include "Config/button_config.h"
#include "BLE/BleKeyboardService.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Forward declarations
class ButtonEventHandler;
class MenuController;
struct HardwareState;

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
 * - Bluetooth (branch)
 *   - Pair (leaf - placeholder, Story 8.2)
 *   - Disconnect (leaf - placeholder, Story 8.3)
 * - Display Off (leaf - DisplayPowerAction, Story 9.5)
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
extern MenuItem bluetoothSubmenu[];
extern MenuItem buttonBehaviorItems[];

// Wheel Mode submenu items (leaf nodes with actions set at runtime)
// Labels derived from WheelMode enum to ensure alignment
inline MenuItem wheelModeSubmenu[] = {
    { wheelModeToDisplayString(WheelMode::SCROLL),  nullptr, nullptr, 0, nullptr, nullptr },
    { wheelModeToDisplayString(WheelMode::VOLUME),  nullptr, nullptr, 0, nullptr, nullptr },
    { wheelModeToDisplayString(WheelMode::ZOOM),    nullptr, nullptr, 0, nullptr, nullptr }
};

inline constexpr uint8_t WHEEL_MODE_COUNT = 3;

// Wheel Direction submenu items (leaf nodes with actions set at runtime)
// Labels derived from WheelDirection enum to ensure alignment
inline MenuItem wheelDirectionSubmenu[] = {
    { wheelDirectionToDisplayString(WheelDirection::NORMAL),    nullptr, nullptr, 0, nullptr, nullptr },
    { wheelDirectionToDisplayString(WheelDirection::REVERSED),  nullptr, nullptr, 0, nullptr, nullptr }
};

inline constexpr uint8_t WHEEL_DIRECTION_COUNT = 2;

// Wheel Behavior submenu (branch nodes - parent menu)
inline MenuItem wheelBehaviorSubmenu[] = {
    { "Wheel Mode",      nullptr, wheelModeSubmenu,      WHEEL_MODE_COUNT,      nullptr, nullptr },
    { "Wheel Direction", nullptr, wheelDirectionSubmenu, WHEEL_DIRECTION_COUNT, nullptr, nullptr }
};

inline constexpr uint8_t WHEEL_BEHAVIOR_COUNT = 2;

// Shared button behavior items - dynamically generated from button action mapping
// Labels populated at runtime from BUTTON_ACTION_DEFINITIONS in initButtonBehaviorMenuItems()
// Parent pointers set at runtime in initMenuTree() based on which button submenu references this
inline MenuItem buttonBehaviorItems[BUTTON_ACTION_COUNT];

inline constexpr uint8_t BUTTON_BEHAVIOR_COUNT = BUTTON_ACTION_COUNT;

// Bluetooth submenu items (leaf nodes with actions set at runtime - Stories 8.2 and 8.3)
inline MenuItem bluetoothSubmenu[] = {
    { "Pair",       nullptr, nullptr, 0, nullptr, nullptr },  // Action filled in Story 8.2
    { "Disconnect", nullptr, nullptr, 0, nullptr, nullptr }   // Action filled in Story 8.3
};

inline constexpr uint8_t BLUETOOTH_SUBMENU_COUNT = 2;

// Button Config submenu (branch nodes - all reference the same shared buttonBehaviorItems)
// Labels set at runtime from BUTTONS[].label in initMenuTree()
// userData stores button index to decouple logic from label strings
// Array size matches BUTTON_COUNT to prevent overflow if button count changes
inline MenuItem buttonConfigSubmenu[BUTTON_COUNT] = {
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr, nullptr },
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr, nullptr },
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr, nullptr },
    { nullptr, nullptr, buttonBehaviorItems, BUTTON_BEHAVIOR_COUNT, nullptr, nullptr }
};

// Main menu items
inline MenuItem mainMenu[] = {
    { "Wheel Behavior", nullptr, wheelBehaviorSubmenu, WHEEL_BEHAVIOR_COUNT, nullptr, nullptr },
    { "Button Config",   nullptr, buttonConfigSubmenu, BUTTON_COUNT, nullptr, nullptr },
    { "Bluetooth",      nullptr, bluetoothSubmenu, BLUETOOTH_SUBMENU_COUNT, nullptr, nullptr },
    { "Display Off",    nullptr, nullptr, 0, nullptr, nullptr },
    { "Device Status",  nullptr, nullptr, 0, nullptr, nullptr },
    { "About",          nullptr, nullptr, 0, nullptr, nullptr }
};

inline constexpr uint8_t MAIN_MENU_COUNT = 6;

// Root node representing the main menu container
inline MenuItem root = { "Menu", nullptr, mainMenu, MAIN_MENU_COUNT, nullptr, nullptr };

// Menu item indices for action assignment
namespace Index {
    inline constexpr uint8_t WHEEL_BEHAVIOR = 0;
    inline constexpr uint8_t BUTTON_CONFIG = 1;
    inline constexpr uint8_t BLUETOOTH = 2;
    inline constexpr uint8_t DISPLAY_OFF = 3;
    inline constexpr uint8_t DEVICE_STATUS = 4;
    inline constexpr uint8_t ABOUT = 5;
}

/**
 * @brief Initialize parent pointers and button labels in the menu tree
 *
 * Must be called once at startup to establish parent-child relationships
 * and set button labels from button_config.h as single source of truth.
 * Parent pointers cannot be constexpr due to circular reference limitations.
 */
/**
 * @brief Initialize button behavior menu items from BLE keyboard service
 *
 * Populates buttonBehaviorItems labels dynamically from service.
 * This enables data-driven menu generation - adding new actions to the service
 * automatically adds them to the menu.
 *
 * Must be called before initMenuTree() to ensure items are initialized.
 *
 * @param bleService BLE keyboard service to query action display names
 */
inline void initButtonBehaviorMenuItems(BleKeyboardService* bleService) {
    for (uint8_t i = 0; i < BUTTON_ACTION_COUNT; i++) {
        ButtonActionId actionId = bleService->getActionIdByIndex(i);
        buttonBehaviorItems[i].label = bleService->getActionDisplayName(actionId);
        buttonBehaviorItems[i].parent = nullptr;  // Set later in initMenuTree
        buttonBehaviorItems[i].children = nullptr;
        buttonBehaviorItems[i].childCount = 0;
        buttonBehaviorItems[i].action = nullptr;  // Set later in initButtonBehaviorActions
    }
}

inline void initMenuTree() {
    // NOTE: initButtonBehaviorMenuItems() must be called BEFORE this function (requires BleKeyboardService)
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
    // userData stores button index to decouple SetButtonBehaviorAction logic from label strings
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        buttonConfigSubmenu[i].parent = &mainMenu[Index::BUTTON_CONFIG];
        buttonConfigSubmenu[i].label = BUTTONS[i].label;
        buttonConfigSubmenu[i].userData = reinterpret_cast<void*>(static_cast<uintptr_t>(i));
    }

    // Set parent for bluetooth submenu items (Pair, Disconnect)
    for (uint8_t i = 0; i < BLUETOOTH_SUBMENU_COUNT; i++) {
        bluetoothSubmenu[i].parent = &mainMenu[Index::BLUETOOTH];
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
 * @brief Set action for a wheel direction menu item
 * @param index Wheel direction index (0=Normal, 1=Reversed)
 * @param action Pointer to MenuAction instance (must outlive menu)
 */
inline void setWheelDirectionAction(uint8_t index, MenuAction* action) {
    if (index < WHEEL_DIRECTION_COUNT) {
        wheelDirectionSubmenu[index].action = action;
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
 * @brief Set action for Display Off menu item
 * @param action Pointer to DisplayPowerAction instance
 */
inline void setDisplayOffAction(MenuAction* action) {
    mainMenu[Index::DISPLAY_OFF].action = action;
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
 * Creates SelectWheelDirectionAction instances for Normal and Reversed directions.
 * Must be called after DI objects (ConfigManager, EncoderModeManager, displayRequestQueue, HardwareState) are created.
 *
 * @param config ConfigManager instance for NVS persistence
 * @param modeMgr EncoderModeManager instance for runtime mode switching
 * @param displayQueue Queue for display refresh on direction change
 * @param hwState HardwareState instance for display state updates (DIP)
 */
inline void initWheelBehaviorActions(ConfigManager* config, EncoderModeManager* modeMgr, QueueHandle_t displayQueue, HardwareState* hwState) {
    // Create static action instances (must outlive menu)
    // HardwareState injected via constructor (Dependency Inversion Principle)
    static SelectWheelModeAction scrollAction(WheelMode::SCROLL, config, modeMgr, hwState);
    static SelectWheelModeAction volumeAction(WheelMode::VOLUME, config, modeMgr, hwState);
    static SelectWheelModeAction zoomAction(WheelMode::ZOOM, config, modeMgr, hwState);

    // Assign actions to wheel mode submenu items
    // Use WheelMode enum values to ensure alignment with menu labels
    setWheelModeAction(static_cast<uint8_t>(WheelMode::SCROLL), &scrollAction);
    setWheelModeAction(static_cast<uint8_t>(WheelMode::VOLUME), &volumeAction);
    setWheelModeAction(static_cast<uint8_t>(WheelMode::ZOOM), &zoomAction);

    // Create static wheel direction action instances
    static SelectWheelDirectionAction normalAction(WheelDirection::NORMAL, config, displayQueue);
    static SelectWheelDirectionAction reversedAction(WheelDirection::REVERSED, config, displayQueue);

    // Assign actions to wheel direction submenu items
    // Use WheelDirection enum values to ensure alignment with menu labels
    setWheelDirectionAction(static_cast<uint8_t>(WheelDirection::NORMAL), &normalAction);
    setWheelDirectionAction(static_cast<uint8_t>(WheelDirection::REVERSED), &reversedAction);
}

/**
 * @brief Initialize bluetooth menu actions
 *
 * Creates PairAction and DisconnectAction instances for Bluetooth menu items.
 * Uses global hardwareState for BLE state tracking.
 *
 * Must be called after DI objects (BleKeyboard, displayQueue) are created.
 *
 * @param bleKeyboard BleKeyboard instance for BLE control
 * @param displayQueue DisplayRequestQueue for user feedback
 */
inline void initBluetoothActions(BleKeyboard* bleKeyboard, QueueHandle_t displayQueue) {
    // Create static action instances (must outlive menu)
    static PairAction pairAction(bleKeyboard, displayQueue);
    static DisconnectAction disconnectAction(bleKeyboard, displayQueue);

    // Assign to Bluetooth submenu items
    bluetoothSubmenu[0].action = &pairAction;       // "Pair" (Story 8.2)
    bluetoothSubmenu[1].action = &disconnectAction; // "Disconnect" (Story 8.3)
}

/**
 * @brief Initialize display menu actions
 *
 * Creates DisplayPowerAction instance for Display Off menu item.
 * Display power is session-only (no NVS persistence) - always starts ON after boot.
 * When display turns OFF, menu automatically exits so controls work immediately.
 *
 * @param display DisplayInterface instance for hardware power control
 * @param menuCtrl MenuController for menu exit when display turns off
 */
inline void initDisplayActions(DisplayInterface* display, MenuController* menuCtrl) {
    // Create static action instance (must outlive menu)
    static DisplayPowerAction displayPowerAction(display, menuCtrl);

    // Assign to Display Off menu item
    setDisplayOffAction(&displayPowerAction);
}

/**
 * @brief Initialize button behavior menu actions
 *
 * Creates context-aware SetButtonBehaviorAction instances for shared button behavior menu items.
 * All buttons share the same behavior items array (None, Mute, Play, Pause, Next, Previous).
 * The button index is determined dynamically at runtime by analyzing the menu navigation context
 * (which button submenu the user navigated through).
 *
 * Must be called after DI objects (ConfigManager, ButtonEventHandler, BleKeyboardService) are created.
 *
 * @param config ConfigManager instance for NVS persistence
 * @param buttonHandler ButtonEventHandler instance for cache invalidation
 * @param bleService BLE keyboard service to get action IDs
 */
inline void initButtonBehaviorActions(ConfigManager* config, ButtonEventHandler* buttonHandler, BleKeyboardService* bleService) {
    // Static storage buffer for action instances (must outlive menu)
    // Using aligned byte array + placement new to avoid hardcoded count
    // This enables true "single source of truth" - adding actions to service auto-scales
    alignas(SetButtonBehaviorAction) static uint8_t actionStorage[BUTTON_ACTION_COUNT * sizeof(SetButtonBehaviorAction)];
    static bool initialized = false;

    if (!initialized) {
        // Construct each action in place using placement new
        // Iterates over service actions dynamically - no hardcoded indices
        for (uint8_t i = 0; i < BUTTON_ACTION_COUNT; i++) {
            ButtonActionId actionId = bleService->getActionIdByIndex(i);
            // Placement new: construct object at specific memory location
            new (&actionStorage[i * sizeof(SetButtonBehaviorAction)]) SetButtonBehaviorAction(
                actionId,
                config,
                buttonHandler,
                bleService
            );
        }
        initialized = true;
    }

    // Cast storage to action pointer for use
    auto* buttonActions = reinterpret_cast<SetButtonBehaviorAction*>(actionStorage);

    // Assign actions to shared button behavior items
    for (uint8_t i = 0; i < BUTTON_BEHAVIOR_COUNT; i++) {
        buttonBehaviorItems[i].action = &buttonActions[i];
    }
}

} // namespace MenuTree
