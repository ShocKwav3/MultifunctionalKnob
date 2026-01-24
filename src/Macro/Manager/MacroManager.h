#pragma once

/**
 * @file MacroManager.h
 * @brief Manager for HID keyboard macro execution
 *
 * MacroManager handles:
 * - Loading/saving macro definitions from/to NVS
 * - Tracking macro button state (held/released)
 * - Executing macros via BleKeyboard when macro mode is active
 */

#include "BleKeyboard.h"
#include <stdint.h>

#include "Config/ConfigManager.h"
#include "Config/macro_config.h"
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
 * 3. Track macro button: setMacroButtonState(held)
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
     * @brief Update macro button state (held/released)
     * @param held true if button is pressed, false if released
     */
    void setMacroButtonState(bool held);

    /**
     * @brief Check if macro mode is currently active
     * @return true if macro button is held
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
    bool macroButtonHeld;                  // Macro button state
    MacroDefinition macros[MACRO_INPUT_COUNT];  // Macro definitions array (static allocation)
};
