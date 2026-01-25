#pragma once

#include <stdint.h>
#include "BleKeyboard.h"

using ButtonActionId = uint8_t;

// Compile-time constant for menu array sizing
// Must match ACTION_COUNT in BleKeyboardService.cpp
constexpr uint8_t BUTTON_ACTION_COUNT = 8;

/**
 * @brief Service layer for BLE keyboard operations
 *
 * Encapsulates media key actions and BLE keyboard execution.
 * Maintains single source of truth for available button actions.
 *
 * Adding new media key actions:
 * 1. Add one line to MEDIA_KEY_ACTIONS[] in BleKeyboardService.cpp
 * 2. Action automatically appears in menus
 * 3. No changes needed to handlers or menu code
 */
class BleKeyboardService {
public:
    explicit BleKeyboardService(BleKeyboard* keyboard);

    /**
     * @brief Execute a media key action by ID
     * @param actionId Button action ID (0 = NONE, 1+ = media keys)
     * @return true if action executed successfully, false if invalid ID or no key assigned
     */
    bool executeMediaKey(ButtonActionId actionId);

    /**
     * @brief Check if BLE keyboard is connected
     * @return true if connected, false otherwise
     */
    bool isConnected() const;

    /**
     * @brief Get display name for menu labels
     * @param actionId Button action ID
     * @return User-facing name (e.g., "Play/Pause") or "Unknown" if invalid
     */
    const char* getActionDisplayName(ButtonActionId actionId) const;

    /**
     * @brief Get confirmation message for menu feedback
     * @param actionId Button action ID
     * @return Confirmation message (e.g., "Play/Pause Assigned") or "Unknown Action"
     */
    const char* getActionConfirmation(ButtonActionId actionId) const;

    /**
     * @brief Get internal identifier for logging
     * @param actionId Button action ID
     * @return Identifier string (e.g., "PLAY_PAUSE") or "UNKNOWN"
     */
    const char* getActionIdentifier(ButtonActionId actionId) const;

    /**
     * @brief Get total number of available actions
     * @return Count of actions (including NONE)
     */
    uint8_t getActionCount() const;

    /**
     * @brief Validate if action ID exists
     * @param actionId Button action ID to check
     * @return true if ID is valid, false otherwise
     */
    bool isValidActionId(ButtonActionId actionId) const;

    /**
     * @brief Get action ID by index (for menu iteration)
     * @param index Array index (0 to getActionCount()-1)
     * @return Action ID at that index, or 0 if index out of range
     */
    ButtonActionId getActionIdByIndex(uint8_t index) const;

private:
    BleKeyboard* bleKeyboard;
};
