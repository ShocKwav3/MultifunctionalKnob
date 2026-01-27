#pragma once

/**
 * @file MacroManager.h
 * @brief Manager for HID keyboard macro execution
 *
 * MacroManager handles:
 * - Loading/saving macro definitions from/to NVS
 * - Tracking macro mode state (toggle ON/OFF)
 * - Executing macros via BleKeyboard when macro mode is active
 */

#include "BleKeyboard.h"
#include <stdint.h>

#include "Config/ConfigManager.h"
#include "Enum/ErrorEnum.h"
#include "Enum/MacroInputEnum.h"
#include "Type/MacroDefinition.h"

/**
 * @class MacroManager
 * @brief Manages macro button state and macro execution
 *
 * Usage:
 * 1. Construct with BleKeyboard pointer
 * 2. Load macros from NVS: loadFromNVS(configManager)
 * 3. Toggle macro mode: toggleMacroMode()
 * 4. Execute on input: if (isMacroModeActive()) executeMacro(input)
 */
class MacroManager {
public:
    /**
     * @brief Construct MacroManager with BLE keyboard dependency
     * @param ble Pointer to BleKeyboard instance for HID output
     */
    explicit MacroManager(BleKeyboard* ble);

    /**
     * @brief Toggle macro mode ON/OFF
     */
    void toggleMacroMode();

    /**
     * @brief Check if macro mode is currently active
     * @return true if macro mode is toggled ON
     */
    bool isMacroModeActive() const;

    /**
     * @brief Execute a macro for the given input
     * @param input Macro input source (WHEEL_BUTTON, BUTTON_1, etc.)
     * @return true if macro was executed, false if skipped (empty macro or BLE disconnected)
     */
    bool executeMacro(MacroInput input);

    /**
     * @brief Load all macro definitions from NVS
     * @param config ConfigManager instance for NVS access
     */
    void loadFromNVS(ConfigManager& config);

    /**
     * @brief Save a macro definition to NVS and update local storage
     * @param config ConfigManager instance for NVS access
     * @param input Macro input slot to save to
     * @param macro MacroDefinition to save
     * @return Error::OK on success, error code on failure
     */
    Error saveMacro(ConfigManager& config, MacroInput input, MacroDefinition macro);

private:
    BleKeyboard* bleKeyboard;              // BLE HID interface (injected)
    bool macroModeActive;                  // Macro mode state (toggle)
    MacroDefinition macros[static_cast<uint8_t>(MacroInput::COUNT)];  // Macro definitions array (static allocation)
};
