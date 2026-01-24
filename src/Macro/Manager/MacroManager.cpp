#include "MacroManager.h"
#include "Config/log_config.h"

static const char* TAG = "MacroManager";

MacroManager::MacroManager(BleKeyboard* ble)
    : bleKeyboard(ble)
    , macroButtonHeld(false)
    , macros{} {  // Zero-initialize array
    LOG_INFO(TAG, "MacroManager initialized");
}

void MacroManager::setMacroButtonState(bool held) {
    macroButtonHeld = held;
}

bool MacroManager::isMacroModeActive() const {
    return macroButtonHeld;
}

bool MacroManager::executeMacro(MacroInput input) {
    uint8_t index = static_cast<uint8_t>(input);

    if (index >= MACRO_INPUT_COUNT) {
        LOG_ERROR(TAG, "Invalid macro input: %d", index);
        return false;
    }

    MacroDefinition& macro = macros[index];

    // Check if macro is empty
    if (macro.isEmpty()) {
        LOG_DEBUG(TAG, "Macro %d is empty, skipping", index);
        return false;
    }

    // Check if BLE is connected
    if (!bleKeyboard->isConnected()) {
        LOG_DEBUG(TAG, "BLE not connected, skipping macro %d", index);
        return false;
    }

    // Execute macro: press modifiers + key, then release
    LOG_DEBUG(TAG, "Executing macro %d: modifiers=0x%02X, keycode=0x%02X",
              index, macro.modifiers, macro.keycode);

    // Send key combination
    // Press each modifier individually (library expects keycodes 0x80-0x87, not bitmask)
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (macro.modifiers & (1 << bit)) {
            bleKeyboard->press(0x80 + bit);
        }
    }

    // Press and release the keycode
    bleKeyboard->press(macro.keycode);
    bleKeyboard->release(macro.keycode);

    // Release each modifier individually
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (macro.modifiers & (1 << bit)) {
            bleKeyboard->release(0x80 + bit);
        }
    }

    return true;
}

void MacroManager::loadFromNVS(ConfigManager& config) {
    uint8_t loadedCount = 0;

    for (uint8_t i = 0; i < MACRO_INPUT_COUNT; i++) {
        MacroDefinition macro;
        Error result = config.loadMacro(i, macro);

        if (result == Error::OK) {
            macros[i] = macro;
            if (!macro.isEmpty()) {
                loadedCount++;
            }
        } else {
            // NVS error - set to empty macro
            LOG_ERROR(TAG, "Failed to load macro %d, setting to empty", i);
            macros[i] = MacroDefinition{0, 0};
        }
    }

    LOG_DEBUG(TAG, "Loaded %d non-empty macros from NVS", loadedCount);
}

Error MacroManager::saveMacro(ConfigManager& config, MacroInput input, MacroDefinition macro) {
    uint8_t index = static_cast<uint8_t>(input);

    if (index >= MACRO_INPUT_COUNT) {
        LOG_ERROR(TAG, "Invalid macro index: %d", index);
        return Error::INVALID_PARAM;
    }

    // Update local array
    macros[index] = macro;

    // Save to NVS
    Error result = config.saveMacro(index, macro.toPacked());

    if (result == Error::OK) {
        LOG_INFO(TAG, "Saved macro %d: modifiers=0x%02X, keycode=0x%02X",
                 index, macro.modifiers, macro.keycode);
    }

    return result;
}
